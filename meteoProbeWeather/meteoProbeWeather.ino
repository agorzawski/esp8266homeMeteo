/**
 * 2017/2018 arek gorzawski
 */
#include "BufferedMeteoData.h"
#include "WifiConnector.h"
#include "WebServerTask.h"
#include "TimerOnChannel.h"
#include "I2cDataCollector.h"
#include "OneWireDataCollector.h"
#include "DisplayTask.h"
#include "DataBufferManager.h"
#include "MqttHandler.h"

#include "utils.h"
#include "task.hpp"
#include "tasks_utils.h"

#include <FS.h>

#define P0 1013.25      // default pressure at the see level
#define PIN_SCL 12      // D6
#define PIN_SDA 13      // D7
#define PIN_LED 2       // D4
#define PIN_ONE_WIRE 0  // D3

#define PIN_RELAY_1 4   // D2
#define PIN_RELAY_2 5   // D1

#define PIN_SPARE_1 16  // D0
#define PIN_SPARE_2 14  // D5
//#define PIN_SCL 15 -> // D8 -> not connected in board v1

using namespace Tasks;
class LedBlinker: public Task
{
  public:
    LedBlinker()
    {
      pinMode(PIN_LED, OUTPUT);
    }

    virtual void run()
    {
      digitalWrite(PIN_LED, s);
      sleep(s ? 2_s - 1: 1);
      s = !s;
    }

  private:
    bool s = false;
};

// ###############################

const static String firmwareVersion = "0.2.2";
const static String boardName = "SondaX";
const static String boardSysName = "sensorX";
const static String topTopic = "home";

// ################################
//Data
//FIXME move data containers to some other place
BufferedMeteoData data;
BufferedMeteoData data1h;
BufferedMeteoData data60h;
DataBufferManager dataBufferManager(data, data1h, data60h);

// Data Collectors
I2cDataCollector tempPressureCollector(PIN_SDA, PIN_SCL);
OneWireDataCollector tempCollector(PIN_ONE_WIRE);

// Network infrastructure
void connectionStateChanged(WifiConnector::States state);
WifiConnector wifiConnector(connectionStateChanged);
void connectionMqttStateChanged(MqttHandler::States state);
MqttHandler mqttHandler(connectionMqttStateChanged, topTopic);
WebServerTask webServerTask;

// Hardware
DisplayTask displayTask(PIN_SDA, PIN_SCL);
LedBlinker ledBlinker;

TimerOnChannel channel1(PIN_RELAY_1, "OnBoard Relay");
//TimerOnChannel channel2(PIN_RELAY_2, "OnBoard LED");

void setup()
{
  Serial.begin(115200);

  checkFileSystem();
	readConfigFromFS();

  logPrintf("[%s/%s] INITIALIZATION STARTED", boardName.c_str(), boardSysName.c_str());
  String macAddress = WiFi.macAddress();
  logPrintf("[MAC Address] %s", macAddress.c_str());

  /* data suppliers */
  tempPressureCollector.registerBuffersData(dataBufferManager);
  tempCollector.registerBuffersData(dataBufferManager);
  /* data consumers */
  webServerTask.registerBuffersData(dataBufferManager);
  webServerTask.setFirmwareVersion(firmwareVersion);
  displayTask.registerBuffersData(dataBufferManager);
  displayTask.setDeviceName(boardName);
  displayTask.setFirmwareVersion(firmwareVersion);

  // // // outside wold communication!
  String boardTopic = topTopic + "/" + boardSysName;
  mqttHandler.setBoardStatusTopic(boardTopic);

  tempPressureCollector.setMqttHandler(mqttHandler, boardTopic);
  channel1.setMqttHandler(mqttHandler, boardTopic + "/relay1");
  tempCollector.setMqttHandler(mqttHandler, boardTopic + "/status/boardSensors");

  /* #############
   TASKS
  these tasks are always running */
  addTask(&ledBlinker);
  addTask(&dataBufferManager);
  addTask(&wifiConnector);
  addTask(&tempPressureCollector);
  addTask(&tempCollector);
  addTask(&displayTask);
  /* and these need to be suspended at the start */
  addTask(&webServerTask);
  webServerTask.suspend();
  addTask(&mqttHandler);
  mqttHandler.suspend();

  logPrintf("[%s/%s] INITIALIZATION DONE! ", boardName.c_str(), boardSysName.c_str());
  setupTasks();
}

void loop()
{
  scheduleTasks();
}

void connectionMqttStateChanged(MqttHandler::States state){
  switch (state)
  {
    case MqttHandler::States::IDLE:
    {
      displayTask.setMqttStatus("IDLE");
      break;
    }
    case MqttHandler::States::CONNECTED:
    {
      displayTask.setMqttStatus("CONN");
      String tmp = topTopic+"/"+boardSysName+"/status/system";
      char _msg[50];
      snprintf (_msg, 50, "{\"ip\":%s, \"essid\": %s, \"firmware\": %s}", WiFi.localIP().toString().c_str(), WiFi.SSID().c_str(), firmwareVersion.c_str());
      mqttHandler.publish( tmp.c_str(), _msg);

      channel1.setOn();
      break;
    }
    case MqttHandler::States::WAITING:
    {
      displayTask.setMqttStatus("WAIT");
      channel1.setOff();
      break;
    }
  }
}

void connectionStateChanged(WifiConnector::States state)
{
  switch (state)
  {
    case WifiConnector::States::NONE:
      webServerTask.suspend();
      return;

    case WifiConnector::States::AP:
    {
      webServerTask.reset();
      webServerTask.resume();
      //TODO manage the essid and pwd for first connection!
      String ip = WiFi.softAPIP().toString();
      logPrintf("[AP IP] = %s", ip.c_str());
      displayTask.setIP(ip);
      displayTask.setSSID(WiFi.SSID());
      return;
    }

    case WifiConnector::States::CLIENT:
    {
      webServerTask.reset();
      webServerTask.resume();
      String ip = WiFi.localIP().toString();
      logPrintf("[Board IP] = %s", ip.c_str());
      displayTask.setIP(ip);
      displayTask.setSSID(WiFi.SSID());

      mqttHandler.reset();
      mqttHandler.resume();
      break;
    }
  }
}

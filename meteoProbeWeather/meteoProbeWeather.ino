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
MqttHandler mqttHandler(connectionMqttStateChanged);
WebServerTask webServerTask;

// Hardware
DisplayTask displayTask(PIN_SDA, PIN_SCL);
LedBlinker ledBlinker;

TimerOnChannel channel1(PIN_RELAY_1, "OnBoard Relay");
TimerOnChannel channel2(PIN_RELAY_2, "OnBoard LED");
TimerOnChannel channel3(PIN_SPARE_1, "ExternalRelay1");

void setup()
{
  Serial.begin(115200);

  if (!SPIFFS.begin())
  {
    SPIFFS.format();
    SPIFFS.begin();
    logPrintf("Formatting filesystem, the default password is %s", readConfig(F("configPassword")).c_str());
    writeConfig(F("configPassword"), F("password"));
  }

  //TODO in to the file
  char* boardName = "Sonda3";
  char* boardSysName = "sensor3";

  logPrintf("[%s/%s] INITIALIZATION STARTED", boardName, boardSysName);
  String macAddress = WiFi.macAddress();
  logPrintf("[MAC Address] %s", macAddress.c_str());

  /* mqtt handler */
  //TODO in to the file
  char* mqtt_server = "192.168.0.125";
  int mqtt_port = 1883;
  char* mqtt_user = "homeassistant";
  char* mqtt_pass = "homeassistant123";
  mqttHandler.setConfiguration(mqtt_server, mqtt_port, mqtt_user, mqtt_pass);

  /* data suppliers */
  tempPressureCollector.registerBuffersData(dataBufferManager);
  // tempCollector.registerBuffersData(dataBufferManager);
  /* data consumers */
  webServerTask.registerBuffersData(dataBufferManager);
  displayTask.registerBuffersData(dataBufferManager);
  displayTask.setDeviceName(boardName);

  // // // outside wold communication!
  // channel1.setMqttHandler(mqttHandler, toCharArray("home/%s/relay1", boardSysName)); // on board relay
  // //channel2.setMqttHandler(mqttHandler, "home/sensor3/relay2"); // only local usavge
  // channel3.setMqttHandler(mqttHandler, toCharArray("home/%s/relay3", boardSysName)); // ext board relay 1
  // tempPressureCollector.setMqttHandler(mqttHandler, toCharArray("home/%s", boardSysName));
  // tempCollector.setMqttHandler(mqttHandler, toCharArray("home/%sb", boardSysName));

  channel1.setMqttHandler(mqttHandler, "home/sensor3/relay1"); // on board relay
  //channel2.setMqttHandler(mqttHandler, "home/sensor3/relay2"); // only local usavge
  channel3.setMqttHandler(mqttHandler, "home/sensor3/relay3"); // ext board relay 1
  tempPressureCollector.setMqttHandler(mqttHandler, "home/sensor3");
  tempCollector.setMqttHandler(mqttHandler,"home/sensor3b");

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

  logPrintf("[%s/%s] INITIALIZATION DONE! ", boardName, boardSysName);
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
      channel2.setOn();
      channel3.setOn();
      break;
    }
    case MqttHandler::States::WAITING:
    {
      displayTask.setMqttStatus("WAIT");
      channel2.setOff();
      channel3.setOff();
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

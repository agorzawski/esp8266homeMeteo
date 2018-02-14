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

TimerOnChannel channel1(PIN_RELAY_1, "Lamp LED");
TimerOnChannel channel2(PIN_RELAY_2, "Lamp LED");
TimerOnChannel channel3(PIN_SPARE_1, "Lamp LED");

void setup()
{
  Serial.begin(115200);
  if (!SPIFFS.begin())
  {
    SPIFFS.format();
    SPIFFS.begin();
    writeConfig(F("configPassword"), F("password"));
    logPrintf("Formatting filesystem, the default password is %s", readConfig(F("configPassword")).c_str());
  }

  // data suppliers
  tempPressureCollector.registerBuffersData(dataBufferManager);
  //  tempCollector.registerBuffersData(dataBufferManager);

  //data consumers
  webServerTask.registerBuffersData(dataBufferManager);
  displayTask.registerBuffersData(dataBufferManager);

  // outside wold communication!
  channel1.setMqttHandler(mqttHandler, "home/meteolamp1/channel1");
  channel2.setMqttHandler(mqttHandler, "home/meteolamp1/channel2");
  channel3.setMqttHandler(mqttHandler, "home/meteolamp1/channel3");
  tempPressureCollector.setMqttHandler(mqttHandler, "home/sensor1");

  //these tasks are always running
  addTask(&ledBlinker);
  addTask(&dataBufferManager);
  addTask(&wifiConnector);
  addTask(&tempPressureCollector);
  //addTask(&tempCollector);
  addTask(&displayTask);
  //and these need to be suspended at the start
  addTask(&webServerTask);
  addTask(&mqttHandler);

  webServerTask.suspend();
  mqttHandler.suspend();
  setupTasks();

  String macAddress = WiFi.macAddress();
  IPAddress ip = WiFi.localIP();
  logPrintf("MAC Address: %s", macAddress.c_str());
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
      channel1.subscribe();
      channel2.subscribe();
      channel3.subscribe();

      channel1.setOn();
      channel2.setOn();
      channel3.setOn();
      break;
    }
    case MqttHandler::States::WAITING:
    {
      displayTask.setMqttStatus("WAIT");
      channel1.setOff();
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
      logPrintf("IP = %s", ip.c_str());
      displayTask.setIP(ip);
      displayTask.setSSID(WiFi.SSID());
      return;
    }

    case WifiConnector::States::CLIENT:
    {
      webServerTask.reset();
      webServerTask.resume();
      String ip = WiFi.localIP().toString();
      logPrintf("IP = %s", ip.c_str());
      displayTask.setIP(ip);
      displayTask.setSSID(WiFi.SSID());
      mqttHandler.reset();
      mqttHandler.resume();
      break;
    }
  }
}

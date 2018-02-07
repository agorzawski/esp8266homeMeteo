/**
 * 2017 arek gorzawski
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

//test

#define P0 1013.25      // default pressure at the see level
#define PIN_SCL 12      // D6
#define PIN_SDA 13      // D7
#define PIN_LED 2       // D4
#define PIN_ONE_WIRE 0  // D3
#define PIN_RELAY_1 4   // D2
#define PIN_RELAY_2 5   // D1
#define PIN_SPARE_1 16  // D0
#define PIN_SPARE_2 14  // D5
//#define PIN_SPARE_3 12// D6
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

//data
//FIXME move data containers to some other place
BufferedMeteoData data;
BufferedMeteoData data1h;
BufferedMeteoData data60h;
DataBufferManager dataBufferManager(data, data1h, data60h);

//collectors & displays
I2cDataCollector tempPressureCollector(PIN_SDA, PIN_SCL);
OneWireDataCollector tempCollector(PIN_ONE_WIRE);
DisplayTask displayTask(PIN_SDA, PIN_SCL);
WebServerTask webServerTask;

// infrastructure
void connectionStateChanged(WifiConnector::States state);
WifiConnector wifiConnector(connectionStateChanged);
MqttHandler mqttHandler;

LedBlinker ledBlinker;

TimerOnChannel channel1(PIN_RELAY_1, "Lamp LED");

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

  channel1.setMqttTopic("home/meteolamp1");
  channel1.setMqttHandler(mqttHandler);

  // data suppliers
  tempPressureCollector.registerBuffersData(dataBufferManager);
  //  tempCollector.registerBuffersData(dataBufferManager);
  tempPressureCollector.setMqttHandler(mqttHandler);
  //data consumers
  webServerTask.registerBuffersData(dataBufferManager);
  displayTask.registerBuffersData(dataBufferManager);

  //these tasks are always running
  addTask(&ledBlinker);
  addTask(&dataBufferManager);
  addTask(&wifiConnector);
  addTask(&tempPressureCollector);
  //  addTask(&tempCollector);
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

// void connectionMqttStateChanged(Mqtthandler::States state){
//   switch (state)
//   {
//     case Mqtthandler::States::NONE:
//     {
//       break;
//     }
//     case Mqtthandler::States::CONNECTED:
//     {
//       break;
//     }
//   }
// }

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

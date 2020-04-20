/**
 /** * 2018 arek gorzawski */

#ifndef MqttHandler_h
#define MqttHandler_h

#include <PubSubClient.h>
#include <string>
#include "task.hpp"
#include <SPI.h>
#include "ESP8266WiFi.h"
#include <DataStore.h>
#include "utils.h"

#define DEFAULT_STATE_PUBLISH_MS 30000
#define DEFAULT_WAITING_MS 60000
#define DEFUALT_RECONNECT_TIME 2500
#define DEFAULT_STATE_UPDATE_DELAY 20

#define DEFAULT_MQTT_PORT 1883

using namespace Tasks;
class MqttHandler : public Task
{
 public:
     enum class States
     {
       NOT_CONFIGURED,
       CONFIGURED,
       CONNECTED,
       WAITING,
       IDLE
     };
     using Callback = void (*)(States state); //main function callback

     MqttHandler(Callback callback, String topTopic):
            _callback(callback)
     {
        _client = new PubSubClient(_espClient);
        _topTopic = topTopic;
     }

     void setBoardStatusTopic(String boardTopTopic)
     {
        _statusTopic = boardTopTopic + "/status/mqtt";
        logPrintf("[MQTT] Established status topic (%s)", _statusTopic.c_str());
     }

     virtual void run()
      {

        if (!_client -> connected() && _mainState == States::IDLE)
        {
          reconnectMqttServer();
        }
        else
        {
          _client -> loop();
        }

        long now = millis();

        if (now - lastMsg > DEFAULT_WAITING_MS && _mainState == States::WAITING)
        {
          updateState(States::IDLE);
        }

        lastMsg = now;
      }

      void reconnectMqttServer() {
        _client -> disconnect();
        auto _mqtt_server = DataStore::value(F("mqttServer"));
        auto _mqtt_user = DataStore::value(F("mqttUser"));
        auto _mqtt_pass = DataStore::value(F("mqttPassword"));
        logPrintf("[MQTT] Connecting (%s, %s)", _mqtt_server.c_str(), _mqtt_user.c_str());
        _client -> setServer(_mqtt_server.c_str(), DEFAULT_MQTT_PORT);
        //_client -> unsubscribe(_topTopic);
        //_client -> subscribe(_topTopic);
        //_client -> setCallback(callbackMqtt);
        _client -> setCallback([this](const char* topic, byte* payload, unsigned int length)
        {
            callbackMqtt(topic, payload, length);
        });

        int index = 0;
        while (!_client->connected()) {
          logPrintf("[MQTT] Attempting connection...");
          if (_clientId == "")
          {
            _clientId = "MeteoProbe-";
            _clientId += String(random(0xffff), HEX);
          }

          if (_client -> connect(_clientId.c_str(), _mqtt_user.c_str(), _mqtt_pass.c_str()))
          {
            logPrintf("[MQTT] Connected");
            updateState(States::CONNECTED);
            _client -> publish(_statusTopic.c_str(), "connected");
            _client -> loop();
          }
          else
          {
            updateState(States::WAITING);
            logPrintf("[MQTT] connection failed, rc=%d try again in 5 seconds", _client->state());
            sleep(5_s);
            index++;
            if (index == 2){
              updateState(States::WAITING);
              return;
            }
          }
        }
      }

      PubSubClient* getHandler()
      {
        return _client;
      }

      boolean isConnected()
      {
        if (_mainState == States::CONNECTED)
        {
          return true;
        }
        return false;
      }

      void publish(const char *topic, const char *payload){
        logPrintf("[MQTT] received: topic (%s) -> (%s)", topic, payload);
        if (_mainState == States::CONNECTED)
        {
          logPrintf("[MQTT] published on topic (%s) value (%s)", topic, payload);
          _client -> publish(topic, payload);
        }
      }

      void subscribe(const char *topic, std::function<void (char *, uint8_t *, unsigned int)> callbackFunction){
        if (_mainState == States::CONNECTED)
        {
          _client -> subscribe(topic);
          Serial.print("[MQTT] subscribed -> "); Serial.println(topic);
        }
      }


private:

  WiFiClient _espClient;
  PubSubClient* _client;
  Callback _callback = nullptr;
  String _clientId = "";
  States _mainState = States::IDLE;
  long lastMsg = 0;
  char msg[50];
  int value = 0;

  String _topTopic = "home";
  String _statusTopic = "status";

  void updateState(States state){
    _mainState = state;
    delay(DEFAULT_STATE_UPDATE_DELAY);
    _callback(state);
  }

  void callbackMqtt(const char* topic, byte* payload, unsigned int length)
  {
   Serial.print("Message arrived [");
   Serial.print(topic);
   Serial.print("] ");
   for (int i=0;i<length;i++) {
     Serial.print((char)payload[i]);
   }
   Serial.println();
  }

};
#endif

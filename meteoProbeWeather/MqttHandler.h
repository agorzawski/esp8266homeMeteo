/**
 /** * 2018 arek gorzawski */

#ifndef MqttHandler_h
#define MqttHandler_h

#include <PubSubClient.h>
#include "task.hpp"
#include <SPI.h>
#include "ESP8266WiFi.h"

#define DEFAULT_STATE_PUBLISH_MS 30000
#define DEFAULT_WAITING_MS 60000
#define DEFUALT_RECONNECT_TIME 2500
#define DEFAULT_STATE_UPDATE_DELAY 20

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

     MqttHandler(Callback callback): _callback(callback)
     {

     }

     void setConfiguration(char* mqtt_server, int mqtt_port, char* mqtt_user, char* mqtt_pass)
     {
       _mqtt_server = mqtt_server;
       _mqtt_port = mqtt_port;
       _mqtt_user = mqtt_user;
       _mqtt_pass = mqtt_pass;
       Serial.println(_mqtt_server);
       logPrintf("[MQTT] %s:%d for %s:%s", _mqtt_server, _mqtt_port, _mqtt_user, _mqtt_pass );
       _mainState = States::CONFIGURED;
     }

     virtual void run()
      {
        if (_mainState == States::CONFIGURED)
        {
          _client = new PubSubClient(_espClient);
          _client -> setServer(_mqtt_server, _mqtt_port);
         //_client -> setCallback(callbackFunction);
         _mainState = States::IDLE;
        }

        if (!_client -> connected() && _mainState == States::IDLE)
        {
          reconnectMqttServer();
        }
        else
        {
          _client -> loop();
        }

        long now = millis();
        if (now - lastMsg > DEFAULT_STATE_PUBLISH_MS && _mainState == States::CONNECTED)
        {
          ++value;
          snprintf(msg, 75, "hello #%ld", value);
          _client -> publish("home/status", msg);
        }

        if (now - lastMsg > DEFAULT_WAITING_MS && _mainState == States::WAITING)
        {
          updateState(States::IDLE);
        }

        lastMsg = now;
      }

      void reconnectMqttServer() {
        int index = 0;
        while (!_client->connected()) {
          logPrintf("[MQTT] Attempting connection...");
          if (_clientId == "")
          {
            _clientId = "MeteoProbe-";
            _clientId += String(random(0xffff), HEX);
          }
          if (_client -> connect(_clientId.c_str(), _mqtt_user, _mqtt_pass))
          {
            logPrintf("[MQTT] Connected");
            updateState(States::CONNECTED);
            _client -> publish("home/status", "connected");
          }
          else
          {
            updateState(States::WAITING);
            logPrintf("[MQTT] connection failed, rc=%d try again in 2 seconds", _client->state());
            delay(DEFUALT_RECONNECT_TIME);
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

      void publish(const char *topic, const char *payload){
        if (_mainState == States::CONNECTED)
        {
          _client -> publish(topic, payload);
        }
      }

      void subscribe(const char *topic, std::function<void (char *, uint8_t *, unsigned int)> callbackFunction){
        if (_mainState == States::CONNECTED)
        {
          _client -> subscribe(topic);
          Serial.print("subscribed -> "); Serial.println(topic);
        }
      }


private:

  States _mainState = States::NOT_CONFIGURED;
  WiFiClient _espClient;
  PubSubClient* _client;
  Callback _callback = nullptr;
  String _clientId = "";

  const char* _mqtt_server = "srv-name-not-set";
  int _mqtt_port = 0;
  const char* _mqtt_user = "user-not-set";
  const char* _mqtt_pass = "pass-not-set";

  long lastMsg = 0;
  char msg[50];
  int value = 0;

  void updateState(States state){
    _mainState = state;
    delay(DEFAULT_STATE_UPDATE_DELAY);
    _callback(state);
  }
};
#endif

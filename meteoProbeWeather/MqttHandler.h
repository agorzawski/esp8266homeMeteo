/**
 /** * 2018 arek gorzawski */

#ifndef MqttHandler_h
#define MqttHandler_h
#include <PubSubClient.h>
#include "task.hpp"
#include <SPI.h>
#include "ESP8266WiFi.h"

using namespace Tasks;
class MqttHandler : public Task
{
 public:
     enum class States
     {
       CONNECTED,
       WAITING,
       IDLE
     };
     using Callback = void (*)(States state);

     MqttHandler(Callback callback): _callback(callback)
     {
      _client = new PubSubClient(_espClient);
      _client -> setServer(mqtt_server, 1883);
      //_client -> setCallback(callbackMqtt);
     }

      virtual void run()
      {
        if (!_client -> connected() && _mainState == States::IDLE) {
          reconnectMqttServer();
        } else {
          _client -> loop();
        }
        long now = millis();

        if (now - lastMsg > 30000 && _mainState == States::CONNECTED) {
          lastMsg = now;
          ++value;
          snprintf (msg, 75, "hello world #%ld", value);
          _client -> publish("homeassistant/all", msg);
        }

        if (now - lastMsg > 30000 && _mainState == States::WAITING) {
          lastMsg = now;
          updateState(States::IDLE);
        }

      }

      void reconnectMqttServer() {
        int index = 0;
        while (!_client->connected()) {
          Serial.print("Attempting MQTT connection...");
          String clientId = "ESP8266Client-";
          clientId += String(random(0xffff), HEX);
          if (_client -> connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
            Serial.println("MQTT Connected");
            _client -> publish("homeassistant/all", "connected");
            updateState(States::CONNECTED);

          } else {
            updateState(States::WAITING);
            Serial.print("failed, rc=");
            Serial.print(_client->state());
            Serial.println(" try again in 2 seconds");
            delay(2500);
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
        if (_mainState == States::CONNECTED){
          _client -> publish(topic, payload);
        } else{

        }
      }

      void subscribe(const char *topic){
        if (_mainState == States::CONNECTED){
          Serial.print("->"); Serial.println(topic);
          _client -> subscribe(topic);
        }
      }


private:

  States _mainState = States::IDLE;
  WiFiClient _espClient;
  PubSubClient* _client;
  Callback _callback = nullptr;

  const char* mqtt_server = "192.168.0.125";
  const char* mqtt_user = "homeassistant";
  const char* mqtt_pass = "homeassistant123";

  long lastMsg = 0;
  char msg[50];
  int value = 0;

  void updateState(States state){
    _mainState = state;
    delay(50);
    _callback(state);
  }

  void callbackMqtt(char* topic, byte* payload, unsigned int length)
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

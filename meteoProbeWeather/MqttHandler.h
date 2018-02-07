/**
 * 2018 arek gorzawski
 */

#ifndef MqttHandler_h
#define MqttHandler_h
#include <PubSubClient.h>
#include "task.hpp"
#include "ESP8266WiFi.h"

using namespace Tasks;
class MqttHandler : public Task
{

 public:

   enum class States
   {
     CONNECTED,
     NONE
   };

      MqttHandler()
      {
        _client = new PubSubClient(_espClient);
        _client->setServer(mqtt_server, 1883);
      }

      virtual void run()
      {
        if (!_client->connected()) {
          reconnectMqttServer();
        }
        mainState = States::CONNECTED;
        _client->loop();
        long now = millis();
        if (now - lastMsg > 2000) {
          lastMsg = now;
          ++value;
          snprintf (msg, 75, "hello world #%ld", value);
          Serial.print("Publish message: ");
          Serial.println(msg);
          _client->publish("homeassistant/all", msg);
        }
      }

      void reconnectMqttServer() {
        // Loop until we're reconnected
        while (!_client->connected()) {
          Serial.print("Attempting MQTT connection...");
          // Create a random client ID
          String clientId = "ESP8266Client-";
          clientId += String(random(0xffff), HEX);
          // Attempt to connect
          if (_client->connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
            Serial.println("MQTT Connected");
            // Once connected, publish an announcement...
            //client.publish("garden/light1/switch", "hello world");
            // ... and resubscribe
            //client.subscribe("garden/light1/switch/set");
            //_client->publish("garden/light1/switch", msg);
          } else {
            Serial.print("failed, rc=");
            Serial.print(_client->state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
          }
        }
      }

      PubSubClient* getHandler()
      {
        return _client;
      }

      void publish(const char *topic, const char *payload){
        if (mainState == States::CONNECTED){
          _client -> publish(topic, payload);
        } else{
          
        }
      }


private:

  States  mainState = States::NONE;
  WiFiClient _espClient;
  PubSubClient* _client;

  const char* mqtt_server = "192.168.0.125";
  const char* mqtt_user = "homeassistant";
  const char* mqtt_pass = "homeassistant123";

  long lastMsg = 0;
  char msg[50];
  int value = 0;
};
#endif

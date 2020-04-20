/**
 * 2016-2018 arek gorzawski
 */

#ifndef TimerOnChannel_h
#define TimerOnChannel_h
#include "Arduino.h"
#include "MqttHandler.h"
#include "utils.h"

class TimerOnChannel
{
  public:
    TimerOnChannel(int pin, String label)
    {
       _pin = pin;
       pinMode(pin, OUTPUT);
       digitalWrite(_pin, LOW);
       _label = label;
    }

    void setMqttHandler(MqttHandler &mqttHandler, String maintopic)
    {
      _mqttHandler = &mqttHandler;

      _mqttTopic = maintopic + "/get";
      logPrintfX(F("[ToC]"), " Publishing for (%s)", _mqttTopic.c_str());

      _mqttTopicSub = maintopic + "/set";
      logPrintfX(F("[ToC]"), " Subscribing for (%s)", _mqttTopicSub.c_str());

      if (_mqttHandler -> isConnected())
      {
        // _mqttHandler -> subscribe(_mqttTopicSub);
      }
    }

    void subscribe()
    {
      // Serial.print("Subscribing for -> "); Serial.println(_mqttTopicSub);
      // if (_mqttHandler -> isConnected())
      // {
      //   _mqttHandler -> subscribe(_mqttTopicSub);
      // }
    }

    void setOn()
    {
      _manual = true;
      digitalWrite(_pin, HIGH);
      if (_mqttHandler != NULL)
      {
        _mqttHandler -> publish(_mqttTopic.c_str(), "ON");
      }
      _isOn = true;
    }

    void setOff()
    {
      _manual = true;
      digitalWrite(_pin, LOW);
      if (_mqttHandler != NULL)
      {
        _mqttHandler -> publish(_mqttTopic.c_str(), "OFF");
      }
      _isOn = false;
    }

    boolean isOn()
    {
      return _isOn;
    }

    int hourOn()
    {
      return _hourOn;
    }

    int hourOff()
    {
      return _hourOff;
    }
    int hourUltimateOff()
    {
      return _hourUltimateOff;
    }

    boolean isManuallyEnabled()
    {
      return _manual;
    }

    void restoreAuto()
    {
      _manual = false;
    }

    boolean isForeseenToBeActive(long timeInMillis)
    {
      int nbOfHours = getNbOfHours(timeInMillis);
      if (_manual)
      {
        return (nbOfHours < _hourUltimateOff);
        //TODO for the time being only simple thing like that, add more clever things for after midnight deadline
      } else // in auto mode
      {
        return ((nbOfHours >= _hourOn) &&  (nbOfHours < _hourOff));
      }
    }

    void adaptStateToConfigurationFor(long timeInMillis)
    {
      if (_hourOn == 0 && _hourOn == 0)
      {
        return;
      }
      if (isForeseenToBeActive(timeInMillis)) // sould be on
      {
        if (!isOn() && !_manual)  // off and auto
        {
              digitalWrite(_pin, HIGH);
              _isOn = true;
        }
      }
      else // should be off
      {
        if (isOn() && !_manual) // on and auto
        {
            digitalWrite(_pin, LOW);
            _isOn = false;
            // switch off and restore auto mode if after ultimate off time!
            if (getNbOfHours(timeInMillis) >= _hourUltimateOff)
            {
              _manual = false;
            }
        }
      }
    }

    String getLabel()
    {
      return _label;
    }
    void updateLabel(String label)
    {
      _label = label;
    }

    void configure(int hourOn, int hourOff, int hourUltimateOff, String dayPattern)
    {
      _hourOn = hourOn;
      _hourOff = hourOff;
      _hourUltimateOff = hourUltimateOff;
      _dayPattern = dayPattern;
    }

    void printStatus()
    {
      Serial.println("--== Pin: "+ String(_pin) + " ==--");
      Serial.println("  ON : "+ String(_hourOn));
      Serial.println(" OFF : "+ String(_hourOff));
      Serial.println("DAYS : "+ _dayPattern);
    }

  private:
    int _pin;
    int _hourOn = -1;
    int _hourOff = -1;
    int _hourUltimateOff = -1;
    String _dayPattern = "11111111";
    String _label = "Lights";
    boolean _manual = false;
    MqttHandler* _mqttHandler = NULL;
    String _mqttTopic;
    String _mqttTopicSub;
    boolean _isOn = false;

   static int getNbOfHours(long timeInMillis)
    {
      return ((timeInMillis  % 86400L) / 3600) + 1; //TODO GMT+1 (GVA)
    }

    // void callbackMqtt(char* topic, byte* payload, unsigned int length)
    // {
    //  Serial.print("[chanell client]: Message arrived [");
    //  Serial.print(topic);
    //  Serial.print("] ");
    //  for (int i=0;i<length;i++) {
    //    Serial.print((char)payload[i]);
    //  }
    //  Serial.println();
    // }


};

#endif

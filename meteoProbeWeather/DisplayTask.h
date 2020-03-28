#include "task.hpp"
using namespace Tasks;

#include "Adafruit_SSD1306.h"
#include "DataBufferManager.h"

#define ONE_DEC 1
#define NO_DEC 0
#define DEFUALT_IDLE_TIME 2000
#define DEFUALT_STARTUP_TIME 10000


class DisplayTask : public Task
{
  public:

    enum class DataDisplay
    {
      TEMPERATURE_IN,
      TEMPERATURE_OUT,
      PRESSURE,
      NONE,
      STARTUP,
    };

    enum class ConfDisplay
    {
      WIFI,
      TEMPERATURE_PROBES
    };

    DisplayTask(int pin_sda, int pin_scl){
      display.begin();
      display.clearDisplay();
    }

    virtual void run()
    {
        if ((millis() - _millisOnLastCheck) > DEFUALT_STARTUP_TIME && _page == DataDisplay::STARTUP)
        {
          _page = DataDisplay::NONE;
        }

        if ((millis() - _millisOnLastCheck) < DEFUALT_STARTUP_TIME && _page == DataDisplay::STARTUP)
        {
          updateDisplay();
          return;
        }

        _page = DataDisplay::NONE;

        if ((millis() - _millisOnLastCheck) > DEFUALT_IDLE_TIME)
        {
          _page = DataDisplay::TEMPERATURE_IN;
          _pageCounter ++;
        }

        if ((millis() - _millisOnLastCheck) > DEFUALT_IDLE_TIME && _pageCounter > 3)
        {
           _page = DataDisplay::PRESSURE;
           _pageCounter = 0;
        }

        if (_page != DataDisplay::NONE)
        {
          updateDisplay();
          _millisOnLastCheck = millis();
          logPrintf("[OLED] Updated and IDLE...");
        }
    }

    void registerBuffersData(DataBufferManager& dataBufferManager)
    {
        _dataBufferManager = &dataBufferManager;
    }

    void setDeviceName(char* name)
    {
      _name = name;
      _millisOnLastCheck = millis();

    }

    void setFirmwareVersion(char* ver){
        _ver = ver;
    }

    void setMqttStatus(String status){
        _mqttStatus = status;
    }

    void setSSID(String ssid)
    {
      _ssid = ssid;
    }

    void setIP(String ip)
    {
      _ip = ip;
    }

    void updateDisplay()
    {
      //TODO get those values from file -> editable from WEB

      float temp = _dataBufferManager->getCurrentData(0);
      float tempTendence = _dataBufferManager->getTendence(0);
      float pressure = _dataBufferManager->getCurrentData(1);
      float pressureTendence = _dataBufferManager->getTendence(1);

      display.clearDisplay();

      if (_page != DataDisplay::STARTUP)
      {
      updateHeader();
      }

      if (_page == DataDisplay::STARTUP)
      {
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setCursor(3,2);
        display.println("MeteoHomeStation");
        display.setTextSize(2);
        display.setCursor(15,20);
        display.print(">");

        display.print(_name);
        display.println("<");
        display.setTextSize(1);
        display.setCursor(30,55);
        display.print("ver. ");
        display.println(_ver);
      }

      if (_page == DataDisplay::TEMPERATURE_IN)
      {
        //FIXME add label get + add label propoer set.
        //_dataBufferManager->getLabel()
        updateDisplayPage("Temp", temp, tempTendence , "C", ONE_DEC);
      }

      if (_page == DataDisplay::PRESSURE)
      {
        updateDisplayPage("Pressure", pressure, pressureTendence, "hPa", NO_DEC);
      }
      display.display();
    }

  private:

    DataDisplay _page = DataDisplay::STARTUP;
    int _pageCounter = 0;
    ConfDisplay _displayWifiConf = ConfDisplay::WIFI;

    DataBufferManager* _dataBufferManager = NULL;
    Adafruit_SSD1306 display;
    long _millisOnLastCheck = 0;
    float _temp = 20.1;
    String _ip = "0.0.0.0";
    String _ssid = "'Set me up' IP!";
    String _mqttStatus = "SET-UP";
    const char* _name = "name-not-set";
    const char* _ver = "ver-not-set";

    void updateHeader()
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      if (_displayWifiConf == ConfDisplay::WIFI)
      {
        display.print("Wifi: "); display.println(_ssid);
        display.print(_ip); display.print("/ "); display.println(_mqttStatus);
      }
    }

    void updateDisplayPage(String desc, float value, float tendence, String unit, int decimalDigit)
    {
        display.setTextSize(2);
        display.setTextColor(WHITE);

        display.setCursor(0,18);
        display.print(desc);

        display.setCursor(5,39);
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.print(value, decimalDigit);
        display.setTextSize(1);
        display.print(unit);

        if (abs(tendence) - 0.015 > 0 )
        {
          display.setCursor(98,15);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.print(tendence, decimalDigit);
          display.setCursor(98,34);
          display.print(_dataBufferManager -> getActualTendenceLabel());
          drawArrow(tendence, 24);
        }
    }

    void drawArrow(float tendence, int arbitY)
    {
      if (abs(tendence) - 0.015 < 0)  // 1deg / h
      {
        display.drawLine(100, arbitY, 120, arbitY, WHITE);
        display.drawLine(115, arbitY-3, 120, arbitY, WHITE);
        display.drawLine(115, arbitY+3, 120, arbitY, WHITE);
      }
      else{
        if (tendence > 0 )
        {
          display.drawLine(100, arbitY+5, 120, arbitY-5, WHITE);
          display.drawLine(115, arbitY-6, 120, arbitY-5, WHITE);
          display.drawLine(117, arbitY+2, 120, arbitY-5, WHITE);
        }
        if (tendence < 0 )
        {
          display.drawLine(100, arbitY-5, 120, arbitY+5, WHITE);
          display.drawLine(115, arbitY-2, 120, arbitY+5, WHITE);
          display.drawLine(117, arbitY+3, 120, arbitY+5, WHITE);
        }
      }
    }

};

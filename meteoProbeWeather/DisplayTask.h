#include "task.hpp"
using namespace Tasks;
#include "Adafruit_SSD1306.h"
#include "DataBufferManager.h"

#define ONE_DEC 1
#define NO_DEC 0

class DisplayTask : public Task
{
  public:

    enum class DataDisplay
    {
      TEMPERATURE_IN,
      TEMPERATURE_OUT,
      PRESSURE,
      NONE
    };

    enum class ConfDisplay
    {
      WIFI,
      TEMPERATURE_PROBES,
      NONE
    };


    DisplayTask(int pin_sda, int pin_scl){
      display.begin();
      display.clearDisplay();
      Serial.begin(115000);
    }

    virtual void run()
    {
        _counter++;
        if (_counter % 5 == 0)
        {
          _page = DataDisplay::PRESSURE;
          _counter = 1;
        }else{
          _page = DataDisplay::TEMPERATURE_IN;
        }
        //logPrintf("Will write something on the OLED screen :) ");
        sleep(2_s);
        updateDisplay();
    }

    void registerBuffersData(DataBufferManager& dataBufferManager)
    {
        _dataBufferManager = &dataBufferManager;
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
      float temp = _dataBufferManager->getCurrentData(0);
      float tempTendence = _dataBufferManager->getTendence(0);

      float pressure = _dataBufferManager->getCurrentData(1);
      float pressureTendence = _dataBufferManager->getTendence(1);

      display.clearDisplay();
      updateHeader();

      if (_page == DataDisplay::TEMPERATURE_IN)
      {
        updateDisplayPage("Indoor", temp, tempTendence , "C", ONE_DEC);
      }

      if (_page == DataDisplay::PRESSURE)
      {
        updateDisplayPage("Pressure", pressure, pressureTendence, "hPa", NO_DEC);
      }

      display.display();
    }


  private:
    int _counter = 0;
    DataDisplay _page = DataDisplay::TEMPERATURE_IN;
    ConfDisplay _displayWifiConf = ConfDisplay::WIFI;
    float _temp = 20.1;
    String _ip = "0.0.0.0";
    String _ssid = "'Set me up' IP!";
    DataBufferManager* _dataBufferManager = NULL;
    Adafruit_SSD1306 display;

    void updateHeader()
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);

      if (_displayWifiConf == ConfDisplay::WIFI)
      {
        display.print("Wifi: "); display.println(_ssid);
        display.println(_ip);
      }
    }

    void updateDisplayPage(String desc, float value, float tendence, String unit, int decimalDigit)
    {
        display.setTextSize(2);
        display.setTextColor(WHITE);

        display.setCursor(0,18);
        display.print(desc);

        display.setCursor(10,39);
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.print(value, decimalDigit);
        display.setTextSize(2);
        display.print(unit);

        if (abs(tendence) - 0.015 > 0 )
        {
          display.setCursor(98,10);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.print(tendence, decimalDigit);
          display.setCursor(98,30);
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

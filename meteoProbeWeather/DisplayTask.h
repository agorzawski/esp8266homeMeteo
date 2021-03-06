#include "task.hpp"
using namespace Tasks;
#include "Adafruit_SSD1306.h"

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
                
        logPrintf("Will write something on the OLED screen :) ");
        sleep(2_s);
        updateDisplay();
    }

    void registerBuffersData(BufferedMeteoData& data)
    {
        _data = &data;    
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

      float temp = _data->getData(0);
      float pressure = _data->getData(1); 
      display.clearDisplay();
      updateHeader();
      
      if (_page == DataDisplay::TEMPERATURE_IN)
      {
        updateDisplayPage("Indoor", temp, "C", 1);
      }
      if (_page == DataDisplay::PRESSURE)
      {
        updateDisplayPage("Pressure", pressure, "mBar", 0);        
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
    BufferedMeteoData* _data = NULL;    
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

    void updateDisplayPage(String desc, float value, String unit, int decimalDigit)
    {
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0,18);
        display.print(desc);     
        display.setCursor(15,41);
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.print(value, decimalDigit);
        display.setTextSize(2); 
        display.print(unit); 
    }
    
};    

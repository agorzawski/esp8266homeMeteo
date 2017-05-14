#include "task.hpp"
using namespace Tasks;
#include "Adafruit_SSD1306.h"

class DisplayTask : public Task
{
  public: 
    
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
          _page = 2;
          _counter = 1;
        }else{
          _page = 1;
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
      if (_page == 1)
      {
        updateDisplayPage("Indoor", temp, "C", 1);
      }
      if (_page == 2)
      {
        updateDisplayPage("Pressure", pressure, "mBar",0);        
      }
           
      display.display();        
    }

    
  private:
    int _counter = 0;
    int _page = 1;
    float _temp = 20.1;
    String _ip = "0.0.0.0";
    String _ssid = "Test_set_me_UP!";
    BufferedMeteoData* _data = NULL;    
    Adafruit_SSD1306 display;

    void updateHeader()
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.print("Wifi "); display.println(_ssid);
      display.println(_ip);      
    }

    void updateDisplayPage(String desc, float value, String unit, int decimalDigit)
    {
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(30,18);
        display.print(desc);     
        display.setCursor(10,41);
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.print(value, decimalDigit); 
        display.print(unit); 
    }
    
};    

#ifndef OLEDDisplay_h
#define OLEDDisplay_h

#include "Adafruit_SSD1306.h"

class OLEDDisplay
{
  public:
    OLEDDisplay()
    {
      display.begin();
      display.clearDisplay();      
    }
    int updateTemperatureDisplay(float temp)
    {
      display.clearDisplay();
      
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.print("Wifi "); display.print(wifiSSID);
    
      display.setCursor(0,10);  
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Mon, 21 May 2017"); 
      
      display.setCursor(10,40);
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.print(temp, 1); 
      display.print("C");  
      
      display.display();
      return -1;     
    }
    
  private:
    Adafruit_SSD1306 display;
    char* wifiSSID = "a_r_o_2";
};

#endif

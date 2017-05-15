#include <Adafruit_GFX.h>
#include <gfxfont.h>

#include "OLEDDisplay.h"
#include "PlotForSSD1306.h"

/*
  REMEMBER that I HARDCODED i2c pins in the Adafruit.cpp file!!, TODO: expose it up!
*/

OLEDDisplay oledDisplay;
PlotForSSD1606 plotter(true, false);
float ts[] = {1.0, 2.5, 3.1, 4.7, 5.1, 6.1, 7.23, 8.23, 9.1, 10.4, 11.3, 12.0, 13.5, 14.1, 15.7, 16.1, 17.1, 18.23, 19.23, 20.1, 21.4, 22};
float data[] = {19.1, 19.5, 19.1, 19.7, 19.1, 18.7, 18.23, 18.53, 18.93, 19.1, 19.5, 19.8, 20.1, 20.5, 21.7, 19.93, 19.53, 19.53, 19.8, 19.9, 19.2, 19.4};

void setup()
{
  Serial.begin(115000);
}

void loop()
{
  //  float temp = 1.0;
  //  for (int i=0; i < 4; i++)
  //  {
  //    oledDisplay.updateTemperatureDisplay(temp);
  //    Serial.println(temp, 1);
  //    delay(2000);
  //    temp += 0.8;
  //  }

  int dataLength = sizeof(ts) / sizeof(*ts);
  plotter.plot(ts, data, dataLength);
  delay(10000);

}

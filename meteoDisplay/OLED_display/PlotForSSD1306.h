/**
   This is a library for ploting simple lines/points fot two arrays provided.
   This library uses Adafruit SSD1306 driver for OLED display.

   2017 arek gorzawski
*/

#ifndef PlotForSSD1606_h
#define PlotForSSD1606_h

#define MAX_SIZE_X 128
#define MAX_SIZE_Y 64

#define USED_SIZE_X 112
#define USED_SIZE_Y 54

#include "Adafruit_SSD1306.h"
#include "CoordinateSystem.h"

class PlotForSSD1606
{
  public :
    PlotForSSD1606(boolean lines, boolean points)
    {
      _lines = lines;
      _points = points;
      display.begin();
      display.clearDisplay();
    }

    void plot(float* timeStamps, float* data, int sizeT) {

      float maxX = maxValue(timeStamps, sizeT);
      float minX = minValue(timeStamps, sizeT);
      float maxY = round(maxValue(data, sizeT));
      float minY = round(minValue(data, sizeT));

      CoordinateSystem newCoordinates(USED_SIZE_X, USED_SIZE_Y, maxX, minX, maxY, minY);
      int xLeftUpperCorner = MAX_SIZE_X - USED_SIZE_X;
      int yLeftUpperCorner = 0;

      display.clearDisplay();
      //axes x & y + min/max/mean values
      display.drawLine(xLeftUpperCorner, 0, MAX_SIZE_X, 0, WHITE);      
      display.drawLine(xLeftUpperCorner, 0, xLeftUpperCorner, USED_SIZE_Y, WHITE);      
      display.drawLine(xLeftUpperCorner, USED_SIZE_Y, MAX_SIZE_X, USED_SIZE_Y, WHITE);
      display.drawLine(MAX_SIZE_X-1, 0, MAX_SIZE_X-1, USED_SIZE_Y, WHITE);

      display.setTextSize(1);
      //x axis
      display.setTextColor(WHITE);
      
      display.setCursor(xLeftUpperCorner, USED_SIZE_Y + 2);
      display.print(minX, 0);
      display.setCursor(MAX_SIZE_X - 15, USED_SIZE_Y + 2);
      display.print(maxX, 0);

      
      //y axis
      display.setCursor(0, 0);
      display.print(maxY, 0);
      
      display.setCursor(0, USED_SIZE_Y - 5);
      display.print(minY, 0);

      display.setCursor(0, USED_SIZE_Y / 2);
      display.print((maxY+minY)/2, 0);
      
      int plotX = 0;
      int plotY = 0;
      for (int i = 0; i < sizeT - 1; i++) {
        plotX = xLeftUpperCorner + newCoordinates.getIntX(timeStamps[i]);
        plotY = yLeftUpperCorner + newCoordinates.getIntY(data[i]);
        if (_lines) {
          display.drawLine(plotX, plotY, xLeftUpperCorner + newCoordinates.getIntX(timeStamps[i + 1]), yLeftUpperCorner + newCoordinates.getIntY(data[i + 1]), WHITE);
        }
        if (_points) {
          display.drawPixel(plotX - 1, plotY, WHITE);
          display.drawPixel(plotX - 2, plotY, WHITE);
          display.drawPixel(plotX - 1, plotY - 1, WHITE);

          display.drawPixel(plotX + 1, plotY, WHITE);
          display.drawPixel(plotX + 2, plotY, WHITE);
          display.drawPixel(plotX + 1, plotY + 1, WHITE);

          display.drawPixel(plotX, plotY - 1, WHITE);
          display.drawPixel(plotX, plotY - 2, WHITE);
          display.drawPixel(plotX + 1, plotY - 1, WHITE);

          display.drawPixel(plotX, plotY + 1, WHITE);
          display.drawPixel(plotX, plotY + 2, WHITE);
          display.drawPixel(plotX - 1, plotY + 1, WHITE);
        }
        display.display();
      }
    }

  private:
    Adafruit_SSD1306 display;
    boolean _lines = true;
    boolean _points = true;

    float minValue(float* a, int sizeT)
    {
      float minVal = a[0];
      for (int i = 0; i < sizeT; i++) {
        if (a[i] < minVal)
          minVal = a[i];
      }
      return minVal;
    }

    float maxValue(float* a, int sizeT)
    {
      float maxVal = a[0];
      for (int i = 0; i < sizeT; i++) {
        if (a[i] > maxVal)
          maxVal = a[i];
      }
      return maxVal;
    }
};

#endif

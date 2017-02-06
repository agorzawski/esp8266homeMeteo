#include "BMP280.h"
#include "MAX44009.h"

#include "Wire.h"
#define P0 1013.25

#define PIN_SCL 12
#define PIN_SDA 13
#define PIN_LED 2

BMP280 tempPresure;
MAX44009 light;

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);

  light.begin(PIN_SDA, PIN_SCL);
  tempPresure.begin(PIN_SDA, PIN_SCL);
  tempPresure.setOversampling(4);

}


void loop()
{
  blinkStatusLED(50,50);
  double T,P;
  char result = tempPresure.startMeasurment();
 
  if(result!=0){
    delay(result);
    result = tempPresure.getTemperatureAndPressure(T,P);
      if(result!=0)
      {
        Serial.print("Temperature = ");Serial.print(T,2); Serial.print(" degC\t\n");
        Serial.print("Pressure = ");Serial.print(P,2); Serial.print(" mBar\t\n");
        Serial.print("Altitude = ");Serial.print(tempPresure.altitude(P,P0),2); Serial.println(" m\n");
      }
  }
  
  blinkStatusLED(50,50);
  
  float lux = light.get_lux();
  Serial.print("Illuminance = ");Serial.print(lux,2); Serial.print(" lux\n");
  
  blinkStatusLED(200, 500);
}


void blinkStatusLED(int high, int low) 
{
  digitalWrite(PIN_LED, HIGH);
  delay(high);
  digitalWrite(PIN_LED, LOW);
  delay(low);
}



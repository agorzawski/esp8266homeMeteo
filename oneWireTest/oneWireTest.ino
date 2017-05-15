//Include libraries
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 0


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup(void)
{
  pinMode(2, OUTPUT);
  Serial.begin(115200); //Begin serial communication
  Serial.println("Arduino Digital Temperature // Serial Monitor Version"); //Print a message
  sensors.begin();
}

void loop(void)
{ 
  blinkStatusLED(200,200);
  // Send the command to get temperatures
  sensors.requestTemperatures();  
  Serial.print("Temperature[1] is: ");
  Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire

  Serial.print("Temperature[2] is: ");
  Serial.println(sensors.getTempCByIndex(1));

  Serial.print("Temperature[3] is: ");
  Serial.println(sensors.getTempCByIndex(2));
  
  //Update value every 1 sec.
  delay(1000);
}

void blinkStatusLED(int high, int low) 
{
  digitalWrite(2, HIGH);
  delay(high);
  digitalWrite(2, LOW);
  delay(low);
}


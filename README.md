# esp8266homeMeteo

A little project that includes:
* `meteoProbeWeather` that is a 'brain for all the input systems'
* ~~`meteoProbe` that can be *stand-alone* or *in slave* mode probe having up to 3 temp sensors and one wind speed/direction meeter.~~
* `meteoDisplay` diffrent approaches on displaying data
* `i2cScanner` simple scanner for i2c bus,

Main part of each component is a wifi board based on ESP-8266EX: https://www.wemos.cc/product/d1-mini.html

Sensors used:
* DS18B20 - Temperature (OneWire)
* GY-BMP280/BME280 - Atmospheric Pressure / Temperature (I2C)
* GY-49 MAX44009 - Ambient Light (I2C)

Display:
* 128X64 OLED Module for I2C

The following schematics are for the PCBs that are used for the `meteoProbeWeather` ~~& `meteoProbe`~~.

`TO BE COMPLETED`

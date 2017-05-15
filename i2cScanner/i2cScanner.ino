//#include "../../esp8266homeMeteo/ledController/TimerOnChannel.h"

#define LED_PIN 2


/*
 * This work is part of the White Rabbit project
 *
 * Copyright (C) 2011,2012 CERN (www.cern.ch)
 * Author: Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 * Author: Grzegorz Daniluk <grzegorz.daniluk@cern.ch>
 *
 * Released according to the GNU GPL, version 2 or any later version.
 */

#define I2C_DELAY 3000

void mi2c_delay(void)
{
  int i;
  for(i=0;i<I2C_DELAY;i++) asm volatile("nop");
}

#define PIN_SCL 12
#define PIN_SDA 13


#define M_SDA_OUT(i, x) {  if(!(x)) { \
  digitalWrite(PIN_SDA, 0); \
  pinMode(PIN_SDA, OUTPUT); \
} else \
  pinMode(PIN_SDA, INPUT); \
  mi2c_delay(); \
}


#define M_SCL_OUT(i, x) {  if(!(x)) { \
  digitalWrite(PIN_SCL, 0); \
  pinMode(PIN_SCL, OUTPUT); \
} else \
  pinMode(PIN_SCL, INPUT); \
  mi2c_delay(); \
}

//#define M_SCL_OUT(i, x) { digitalWrite(PIN_SCL, (x)? HIGH:LOW); mi2c_delay(); }
#define M_SDA_IN(i) (digitalRead(PIN_SDA) == HIGH ? 1 : 0)

void mi2c_start(uint8_t i2cif)
{
  M_SDA_OUT(i2cif, 0);
  M_SCL_OUT(i2cif, 0);
}

void mi2c_repeat_start(uint8_t i2cif)
{
  M_SDA_OUT(i2cif, 1);
  M_SCL_OUT(i2cif, 1);
  M_SDA_OUT(i2cif, 0);
  M_SCL_OUT(i2cif, 0);
}

void mi2c_stop(uint8_t i2cif)
{
  M_SDA_OUT(i2cif, 0);
  M_SCL_OUT(i2cif, 1);
  M_SDA_OUT(i2cif, 1);
}

unsigned char mi2c_put_byte(uint8_t i2cif, unsigned char data)
{
  char i;
  unsigned char ack;

  for (i = 0; i < 8; i++, data <<= 1) {
    M_SDA_OUT(i2cif, data & 0x80);
    M_SCL_OUT(i2cif, 1);
    M_SCL_OUT(i2cif, 0);
  }

  M_SDA_OUT(i2cif, 1);
  M_SCL_OUT(i2cif, 1);

  ack = M_SDA_IN(i2cif);  /* ack: sda is pulled low ->success.     */

  M_SCL_OUT(i2cif, 0);
  M_SDA_OUT(i2cif, 0);

  return ack != 0;
}

void mi2c_get_byte(uint8_t i2cif, unsigned char *data, uint8_t last)
{

  int i;
  unsigned char indata = 0;

  M_SDA_OUT(i2cif, 1);
  /* assert: scl is low */
  M_SCL_OUT(i2cif, 0);

  for (i = 0; i < 8; i++) {
    M_SCL_OUT(i2cif, 1);
    indata <<= 1;
    if (M_SDA_IN(i2cif))
      indata |= 0x01;
    M_SCL_OUT(i2cif, 0);
  }

  if (last) {
    M_SDA_OUT(i2cif, 1);  //noack
    M_SCL_OUT(i2cif, 1);
    M_SCL_OUT(i2cif, 0);
  } else {
    M_SDA_OUT(i2cif, 0);  //ack
    M_SCL_OUT(i2cif, 1);
    M_SCL_OUT(i2cif, 0);
  }

  *data = indata;
}

void mi2c_init(uint8_t i2cif)
{
  M_SCL_OUT(i2cif, 1);
  M_SDA_OUT(i2cif, 1);
}

uint8_t mi2c_devprobe(uint8_t i2cif, uint8_t i2c_addr)
{
  uint8_t ret;
  mi2c_start(i2cif);
  ret = !mi2c_put_byte(i2cif, i2c_addr << 1);
  mi2c_stop(i2cif);

  return ret;
}

void mi2c_scan(uint8_t i2cif)
{
    int i;

    for(i=0;i<0x80;i++)
    {
     mi2c_start(i2cif);
     if(!mi2c_put_byte(i2cif, i<<1)) Serial.printf("found : %x\n", i);
     mi2c_stop(i2cif);

    }
//    pp_printf("Nothing more found...\n");
}



void blinkStatusLED(int high, int low) 
{
  digitalWrite(LED_PIN, HIGH);
  delay(high);
  digitalWrite(LED_PIN, LOW);
  delay(low);
}

#define MAX44009_I2C_ADDR 0x4a

#define MAX44009_CONFIG 0x2
#define MAX44009_LUX_HI 0x3
#define MAX44009_LUX_LO 0x4

void max44009_write_reg( uint8_t reg, uint8_t val )
{
  mi2c_start(0);
  mi2c_put_byte(0,MAX44009_I2C_ADDR << 1);
  mi2c_put_byte(0, reg);
  mi2c_put_byte(0, val);
  mi2c_stop(0);
}

uint8_t max44009_read_reg( uint8_t reg )
{
  mi2c_start(0);
  mi2c_put_byte(0,MAX44009_I2C_ADDR << 1);
  mi2c_put_byte(0, reg);
  mi2c_repeat_start(0);
  mi2c_put_byte(0, (MAX44009_I2C_ADDR << 1) | 1);
  uint8_t val;
  mi2c_get_byte(0, &val, 1);
  mi2c_stop(0);

  return val;
}


void loop() {
  // put your main code here, to run repeatedly:
  blinkStatusLED(100, 100);

  uint16_t hi = max44009_read_reg(MAX44009_LUX_HI);
  uint16_t lo = max44009_read_reg(MAX44009_LUX_LO);
  Serial.printf("hi  %x lo %x\n", hi, lo);
}


void setup() {

  pinMode(LED_PIN, OUTPUT);
  pinMode(PIN_SCL, OUTPUT);
  pinMode(PIN_SDA, INPUT);
  Serial.begin(115200);

  Serial.printf("\n\nI2c scan\n");
  mi2c_init(0);
  mi2c_scan(0);

  
  
  digitalWrite(LED_PIN, LOW);
//  TimerOnChannel(4,"Terace LEDs")

}


#include "Arduino.h"

#define MAX44009_I2C_ADDR 0x4a

#define MAX44009_CONFIG 0x2
#define MAX44009_LUX_HI 0x3
#define MAX44009_LUX_LO 0x4

#define I2C_DELAY 3000

class MAX44009
{
  public:
    MAX44009();
    void begin(int sdaPin, int sclPin);
    float get_lux(void);

  private:
  
    int _sdaPin;
    int _sclPin;

    void max44009_write_reg( uint8_t reg, uint8_t val );
    uint8_t max44009_read_reg( uint8_t reg );
        
    /*
     * This work is part of the White Rabbit project
     *
     * Copyright (C) 2011,2012 CERN (www.cern.ch)
     * Author: Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
     * Author: Grzegorz Daniluk <grzegorz.daniluk@cern.ch>
     *
     * Released according to the GNU GPL, version 2 or any later version.
     */
  
    void mi2c_delay(void)
    {
      int i;
      for(i=0;i<I2C_DELAY;i++) asm volatile("nop");
    }
    
    #define M_SDA_OUT(i, x) {  if(!(x)) { \
      digitalWrite(_sdaPin, 0); \
      pinMode(_sdaPin, OUTPUT); \
    } else \
      pinMode(_sdaPin, INPUT); \
      mi2c_delay(); \
    }
    
    
    #define M_SCL_OUT(i, x) {  if(!(x)) { \
      digitalWrite(_sclPin, 0); \
      pinMode(_sclPin, OUTPUT); \
    } else \
      pinMode(_sclPin, INPUT); \
      mi2c_delay(); \
    }
    
    #define M_SDA_IN(i) (digitalRead(_sdaPin) == HIGH ? 1 : 0)
    
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

};

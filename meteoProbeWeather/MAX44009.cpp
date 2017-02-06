#include "MAX44009.h"

MAX44009::MAX44009()
{
}

void MAX44009::begin(int sda, int scl)
{
  _sdaPin = sda;
  _sclPin = scl;
}

float MAX44009::get_lux(void)
{
  int luxHigh = max44009_read_reg(MAX44009_LUX_HI); 
  int luxLow = max44009_read_reg(MAX44009_LUX_LO);
  int exponent = (luxHigh & 0xf0) >> 4;
  int mant = (luxHigh & 0x0f) << 4 | luxLow;  
  return (float)(((0x00000001 << exponent) * (float) mant) * 0.045);
}

void MAX44009::max44009_write_reg( uint8_t reg, uint8_t val)
{
  mi2c_start(0);
  mi2c_put_byte(0,MAX44009_I2C_ADDR << 1);
  mi2c_put_byte(0, reg);
  mi2c_put_byte(0, val);
  mi2c_stop(0);
}

uint8_t MAX44009::max44009_read_reg( uint8_t reg )
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

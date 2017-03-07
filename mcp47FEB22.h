/**
Arduino library for MicroChip MCP47FEB22 I2C D/A converter.
*/

#ifndef mcp47FEB22_h
#define mcp47FEB22_h

#include "Arduino.h"
#include <Wire.h>

#define defaultVDD 5000
#define BASE_ADDR 0x60
#define RESET 0B00000110
#define WAKE 0B00001010
#define UPDATE 0B00001000
#define GENERALCALL 0B0000000
#define READ 0B00000110
#define WRITE 0B00000000
#define DAC0_REG 0B00000000
#define DAC1_REG 0B00000001
#define VREF_REG 0B00001000
#define PD_REG 0B00001001
#define GAIN_REG 0B00001010
#define WL_REG 0B000001011
#define DAC0_EP_REG 0B00010000
#define DAC1_EP_REG 0B00010001
#define VREF_EP_REG 0B00011000
#define PD_EP_REG 0B00011001
#define GAIN_EP_REG 0B00011010


class mcp47FEB22
{
  public:
    mcp47FEB22(uint8_t);
    void     vdd(uint16_t);
    void     begin();
    uint8_t  reset();
    uint8_t  wake();
    uint8_t  update();
    uint8_t  analogWrite(uint16_t, uint16_t);
    uint8_t  eepromWrite();
    uint8_t  eepromReset();
    void     print();
    uint8_t  setVref(uint8_t, uint8_t);
    uint8_t  setGain(uint8_t, uint8_t);
    uint8_t  setPowerDown(uint8_t, uint8_t);
    uint8_t  getId();
    uint8_t  getVref(uint8_t);
    uint8_t  getGain(uint8_t);
    uint8_t  getPowerDown(uint8_t);
    uint16_t getValue(uint8_t);
    uint8_t  getVrefEp(uint8_t);
    uint8_t  getGainEp(uint8_t);
    uint8_t  getPowerDownEp(uint8_t);
    uint16_t getValueEp(uint8_t);
    uint16_t getVout(uint8_t);

  private:
    void         getStatus();
    uint8_t      fastWrite(uint8_t, uint16_t);
    uint8_t      writeVref();
    uint8_t      writeGain();
    uint8_t      writePowerDown();
    uint8_t      writeVout();
    uint8_t      _dev_address;
    uint8_t      _deviceID;
    uint8_t      _intVref[2];
    uint8_t      _gain[2];
    uint8_t      _powerDown[2];
    uint16_t     _values[2];
    uint16_t     _valuesEp[2];
    uint8_t      _intVrefEp[2];
    uint8_t      _gainEp[2];
    uint8_t      _powerDownEp[2];
    uint8_t      _wiperLock[2];
    uint16_t     _vdd;
    uint8_t      _simpleCommand(byte);
    uint8_t      _return;
};
#endif


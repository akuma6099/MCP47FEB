/*

  mcp47FEB22.cpp - Arduino library for MicroChip MCP47FEB22 I2C D/A converter
  For implementation details, please take a look at the datasheet http://ww1.microchip.com/downloads/en/DeviceDoc/20005375A.pdf
  For discussion and feedback, please go to http://arduino.cc/forum/index.php/topic,51842.0.html
*/


/* _____PROJECT INCLUDES_____________________________________________________ */
#include "mcp47FEB22.h"


/* _____PUBLIC FUNCTIONS_____________________________________________________ */
/**
Constructor.

Creates class object. Initialize buffers
*/
mcp47FEB22::mcp47FEB22(uint8_t deviceID = 0x00)
{
  _deviceID = deviceID;
  _dev_address = (BASE_ADDR | _deviceID);
  _vdd = defaultVDD;
}
/*
Begin I2C, get current values (input register and eeprom) of mcp47FEB22
*/
void mcp47FEB22::begin()
{
  Wire.begin();
  getStatus();
}
/*
General Call Reset of mcp47FEB22 - EEPROM values will be loaded into the input registers.
*/
uint8_t mcp47FEB22::reset() {
  return _simpleCommand(RESET);
}
/*
General Call Wake-Up of mcp47FEB22 - Reset Power-Down bits (PD0,PD1 = 0,0). refer to DATASHEET 5.4.2
*/
uint8_t mcp47FEB22::wake() {
  return _simpleCommand(WAKE);
}
/*
Write Vout0/Vout1 register values using fastwrite method.
Values : 0-4095 (12-Bits hence the & 0x0FFF)
*/
uint8_t mcp47FEB22::analogWrite(uint16_t value1, uint16_t value2) {
  _values[0] = word((value1 & 0x0FFF));
  _values[1] = word((value2 & 0x0FFF));
  _return = 0;
  _return |= fastWrite(DAC0_REG, _values[0]);
  _return |= fastWrite(DAC1_REG, _values[1]);
  return _return;
}
/*
Write all EEPROM values in order. Requires slight delay between EEPROM write cycles.
delay(50) is enough, 100 is safest.
*/
uint8_t mcp47FEB22::eepromWrite()
{
  _valuesEp[0] = _values[0]; 
  _valuesEp[1] = _values[1];
  _intVrefEp[0] = _intVref[0];
  _intVrefEp[1] = _intVref[1];
  _gainEp[0] = _gain[0];
  _gainEp[1] = _gain[1];
  _powerDownEp[0] = _powerDown[0];
  _powerDownEp[1] = _powerDown[1];
  _return = 0;
  _return |= fastWrite(DAC0_EP_REG, _valuesEp[0]);
  delay(100);
  _return |= fastWrite(DAC1_EP_REG, _valuesEp[1]);
  delay(100);
  _return |= fastWrite(VREF_EP_REG, word(_intVrefEp[0] | _intVrefEp[1]));
  delay(100);
  _return |= fastWrite(GAIN_EP_REG, word((_gainEp[0] | _gainEp[1]) << 8));
  delay(100);
  _return |= fastWrite(PD_EP_REG, word(_powerDownEp[0] | _powerDownEp[1]));
  delay(100);
  return _return;
}
/*
Reset EEPROM and input registers to factory defaults. Refer datasheet TABLE 4-2
Input value = 0, Voltage Reference = 0 (VDD), Gain = 0, PowerDown = 0
*/
uint8_t mcp47FEB22::eepromReset()
{
  _values[0] = _values[1] = 0;
  _intVref[0] = _intVref[1] = 0;
  _gain[0] = _gain[1] = 0;
  _powerDown[0] = _powerDown[1] = 0;
  return eepromWrite();
}

/* 
  Write Voltage reference settings to input regiters
    Vref setting = 1 (internal), Gain = 0 (x1)   ==> Vref = 2.048V
    Vref setting = 1 (internal), Gain = 1 (x2)   ==> Vref = 4.096V
    Vref setting = 0 (external), Gain = ignored  ==> Vref = VDD
*/

uint8_t mcp47FEB22::setVref(uint8_t value1, uint8_t value2) {
  _intVref[0] = value1;
  _intVref[1] = value2 << 2;
  return writeVref();
}
/* 
  Write Gain settings to input regiters
    Vref setting = 1 (internal), Gain = 0 (x1)   ==> Vref = 2.048V
    Vref setting = 1 (internal), Gain = 1 (x2)   ==> Vref = 4.096V
    Vref setting = 0 (external), Gain = ignored  ==> Vref = VDD
*/
uint8_t mcp47FEB22::setGain(uint8_t value1, uint8_t value2) {
  _gain[0] = value1;
  _gain[1] = value2 << 1;
  return writeGain();
}
/*
  Write Power-Down settings to input registers
    0 = Normal , 1-3 = shut down most channel circuit, no voltage out and saving some power.
    1 = 1K ohms to GND, 2 = 100K ohms to GND, 3 = 500K ohms to GND
*/

uint8_t mcp47FEB22::setPowerDown(uint8_t value1, uint8_t value2) {
  _powerDown[0] = value1;
  _powerDown[1] = value2 << 2;
  return writePowerDown();
}
/*
  Return Device ID (up to 8 devices can be used in a I2C bus, Device ID = 0-7)
*/
uint8_t mcp47FEB22::getId()
{
  return _deviceID;
}
/*
  Return Voltage Rerference setting
*/
uint8_t mcp47FEB22::getVref(uint8_t channel)
{
    switch(channel)
    {
        case 0:
            return _intVref[channel];
            break;
        case 1:
            return _intVref[channel] >> 2;
            break;
        default:
            _intVref[channel];
            break;
    }
}
/*
  Return Gain setting
*/
uint8_t mcp47FEB22::getGain(uint8_t channel)
{
    switch(channel)
    {
        case 0:
            return _gain[channel];
            break;
        case 1:
            return _gain[channel] >> 1;
            break;
        default:
            return _gain[channel];
            break;
    }
}
/*
  Return PowerDown setting
*/
uint8_t mcp47FEB22::getPowerDown(uint8_t channel)
{
    switch(channel)
    {
        case 0:
            return _powerDown[channel];
            break;
        case 1:
            return _powerDown[channel] >> 2;
            break;
        default:
            return _powerDown[channel];
            break;
    }
}
/*
  Return Input Regiter value
*/
uint16_t mcp47FEB22::getValue(uint8_t channel)
{
  return _values[channel];
}
/*
  Return EEPROM Voltage Reference setting
*/
uint8_t mcp47FEB22::getVrefEp(uint8_t channel)
{
    switch(channel)
    {
        case 0:
            return _intVrefEp[channel];
            break;
        case 1:
            return _intVrefEp[channel] >> 2;
            break;
        default:
            return _intVrefEp[channel];
            break;
    }
}
/*
  Return EEPROM Gain setting
*/
uint8_t mcp47FEB22::getGainEp(uint8_t channel)
{
    switch(channel)
    {
        case 0:
            return _gainEp[channel];
            break;
        case 1:
            return _gainEp[channel] >> 1;
            break;
        default:
            return _gainEp[channel];
            break;
    }
}
/*
  Return EEPROM PowerDown setting
*/
uint8_t mcp47FEB22::getPowerDownEp(uint8_t channel)
{
    switch(channel)
    {
        case 0:
            return _powerDownEp[channel];
            break;
        case 1:
            return _powerDownEp[channel] >> 2;
            break;
        default:
            return _powerDownEp[channel];
            break;
    }
}
/*
  Return EEPROM value
*/
uint16_t mcp47FEB22::getValueEp(uint8_t channel)
{
  return _valuesEp[channel];
}
/*
  Set VDD for Vout calculation
*/
void mcp47FEB22::vdd(uint16_t currentVdd)
{
  _vdd = currentVdd;
}

/* _____PRIVATE FUNCTIONS_____________________________________________________ */

/*
Get current values (input register and eeprom) of mcp47FEB22
*/
void mcp47FEB22::getStatus()
{
    //Volatile Registers
  for (uint8_t channel=0; channel <= 1; channel++) {
      //DAC Outputs
      Wire.beginTransmission(_dev_address);
      Wire.write(READ | channel << 3);  //Bits 7-2 = address (0x0FFF Vol, 0x1FFF non-Vol), Bits 2-1 = command bits (11 = read) Bit 0=x.
      Wire.endTransmission(false);
      Wire.requestFrom((int)_dev_address, 2);
      int hiByte = Wire.read();
      int loByte = Wire.read();
      _values[channel] = word((hiByte & 0B00001111), loByte);
      Wire.endTransmission();
      
  }
  
   //Vref
  Wire.beginTransmission(_dev_address);
  Wire.write(READ | VREF_REG << 3);
  Wire.endTransmission(false);
  Wire.requestFrom((int)_dev_address, 2);
  while(Wire.available())
  {
      int hiByte = Wire.read();
      int loByte = Wire.read();
      _intVref[0] = (loByte & 0B00000011);
      _intVref[1] = (loByte & 0B00001100);
  }
  Wire.endTransmission();
  
  //Power-Down
  Wire.beginTransmission(_dev_address);
  Wire.write(READ | PD_REG << 3);
  Wire.endTransmission(false);
  Wire.requestFrom((int)_dev_address, 2);
  while(Wire.available())
  {
      int hiByte = Wire.read();
      int loByte = Wire.read();
      _powerDown[0] = (loByte & 0B00000011);
      _powerDown[1] = (loByte & 0B00001100);
  }
  Wire.endTransmission();
  
  //Gain
  Wire.beginTransmission(_dev_address);
  Wire.write(READ | GAIN_REG << 3);
  Wire.endTransmission(false);
  Wire.requestFrom((int)_dev_address, 2);
  while(Wire.available())
  {
      int hiByte = Wire.read();
      int loByte = Wire.read();
      _gain[0] = (hiByte & 0B00000001);
      _gain[1] = (hiByte & 0B00000010);
  }
  Wire.endTransmission();
  
  //WiperLock
  Wire.beginTransmission(_dev_address);
  Wire.write(READ | WL_REG << 3);
  Wire.endTransmission(false);
  Wire.requestFrom((int)_dev_address, 2);
  while(Wire.available())
  {
      int hiByte = Wire.read();
      int loByte = Wire.read();
      _wiperLock[0] = (loByte & 0B00000011);
      _wiperLock[1] = (loByte & 0B00001100);
  }
  Wire.endTransmission();
  
  //Non-Volatile Registers (EEPROM)
  for (uint8_t channel=0; channel <= 1; channel++) {
      //DAC Outputs
      Wire.beginTransmission(_dev_address);
      Wire.write(0x80 | (READ | channel << 3));  //Bits 7-2 = address (0x0FFF Vol, 0x1FFF non-Vol), Bits 2-1 = command bits (11 = read) Bit 0=x.
      Wire.endTransmission(false);
      Wire.requestFrom((int)_dev_address, 2);
      int hiByte = Wire.read();
      int loByte = Wire.read();
      _valuesEp[channel] = word((hiByte & 0B00001111), loByte);
      Wire.endTransmission();
      
  }
  
   //Vref
  Wire.beginTransmission(_dev_address);
  Wire.write(0x80 | (READ | VREF_REG << 3));
  Wire.endTransmission(false);
  Wire.requestFrom((int)_dev_address, 2);
  while(Wire.available())
  {
      int hiByte = Wire.read();
      int loByte = Wire.read();
      _intVrefEp[0] = (loByte & 0B00000011);
      _intVrefEp[1] = (loByte & 0B00001100);
  }
  Wire.endTransmission();
  
  //Power-Down
  Wire.beginTransmission(_dev_address);
  Wire.write(0x80 | (READ | PD_REG << 3));
  Wire.endTransmission(false);
  Wire.requestFrom((int)_dev_address, 2);
  while(Wire.available())
  {
      int hiByte = Wire.read();
      int loByte = Wire.read();
      _powerDownEp[0] = (loByte & 0B00000011);
      _powerDownEp[1] = (loByte & 0B00001100);
  }
  Wire.endTransmission();
  
  //Gain
  Wire.beginTransmission(_dev_address);
  Wire.write(0x80 | (READ | GAIN_REG << 3));
  Wire.endTransmission(false);
  Wire.requestFrom((int)_dev_address, 2);
  while(Wire.available())
  {
      int hiByte = Wire.read();
      int loByte = Wire.read();
      _gainEp[0] = (hiByte & 0B00000001);
      _gainEp[1] = (hiByte & 0B00000010);
  }
  Wire.endTransmission();
  
}
/*
FastWrite
*/
uint8_t mcp47FEB22::fastWrite(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(_dev_address);
  Wire.write(WRITE | reg << 3);
  Wire.write((value >> 8) & 0xFF);
  Wire.write(value & 0xFF);
  return Wire.endTransmission();
}
/*
Write Voltage reference setting to input registers. refer to DATASHEET 5.6.5
No EEPROM update
*/
uint8_t mcp47FEB22::writeVref() {
  Wire.beginTransmission(_dev_address);
  Wire.write(WRITE | VREF_REG << 3);
  Wire.write(0x00);
  Wire.write(_intVref[0] | _intVref[1]);
  return Wire.endTransmission();
}
/*
Write Gain setting to input registers. refer to DATASHEET 5.6.7
No EEPROM update
*/
uint8_t mcp47FEB22::writeGain() {
  Wire.beginTransmission(_dev_address);
  Wire.write(WRITE | GAIN_REG << 3);
  Wire.write(_gain[0] | _gain[1]);
  Wire.write(0x00);
  return Wire.endTransmission();
}
/*
Write PowerDown setting to input registers. refer to DATASHEET 5.6.6
No EEPROM update
*/
uint8_t mcp47FEB22::writePowerDown() {
  Wire.beginTransmission(_dev_address);
  Wire.write(WRITE | PD_REG << 3);
  Wire.write(0x00);
  Wire.write(_powerDown[0] | _powerDown[1]);
  return Wire.endTransmission();
}
/*
Calculate Voltage out based on current setting of Vref and gain
No EEPROM update

void mcp47FEB22::writeVout()
{
  for (uint8_t channel=0; channel <= 1; channel++) {
    if (_intVref[channel] == 1) {
      _values[channel] = _vOut[channel] / (_gain[channel] + 1) * 2;
    }
    else {
      _values[channel] = (long(_vOut[channel]) * 4096) / _vdd ;
    }
  }
  fastWrite();
}*/
/*
Common function for simple general commands
*/
uint8_t mcp47FEB22::_simpleCommand(byte simpleCommand) {
  Wire.beginTransmission(GENERALCALL);
  Wire.write(simpleCommand);
  return Wire.endTransmission();
}
# MCP47FEB
Arduino Library for the MCP47FEB22 DAC by Microchip

This is a work in progress of migrating an existing library created by Neuroelec for the MCP4728 DAC.
MCP4728: https://code.google.com/archive/p/neuroelec/downloads

Made minor code corrections as well as re-implemented reading and writing routines due to some differences in registers and memory maps. Had to bit-shift, and/or alot...

The MCP47FEB22 is a 12-Bit Dual channel DAC controlled via I2C.
-Has Non-Vol storage to retain outputs, gain, and powerdown states.
-3 Selectable Vref Sources (VDD, Ext VRef pin, or Internal Band-Gap 1.22v)
-Setting time of 6us.
-Output gain
-POR/BOR

Datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/20005375A.pdf


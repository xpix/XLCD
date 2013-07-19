XStepperLCD
===========

LCD Module for XStepper

Some time ago, we designed and built version 1.0 of Xstepper.  Unlike other existing GRBL-based CNC solutions, Xstepper is an all-in-one device.  The possibility of controlling the machine in a headless state may be possible, and the first step toward doing so is some sort of visual display.  This module aims at solving part of the problem!

But how?  The good people coding GRBL don’t have the on-chip memory to code in an LCD interface for GRBL.  Add to that the severe lack of available IO and this seems to rule out this type of expansion.  However, there is always a workaround.

This module spies on the serial communication between the arduino and PC using an ATtiny85.  Simultaneously the ATtimy polls the position of the spindle from the XStepper.  The returned data is then parsed and displayed on a local LCD.  You can choose to display the debug screen, work position or machine position using the on-board button.

![sketchup preview](https://raw.github.com/xpix/XStepperLCD/master/eagleUp/sketchup.JPG)


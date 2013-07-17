XStepperLCD
===========

LCD Module for XStepper

Sometimes ago we build the version 1.0 from XSTepper, to control CNC Machines with grbl. It's an all in one device, but to control this machine maybe headless we need first an LCD Display!

But how? The Friends they coded grbl have no space and time to include a library to display Data on an LCD interface, also they use ever pin from the arduino machine and here comes my Idea :)

This module monitor/spy/listen on the serial communication between arduino and pc. He search with an Attiny85 for a special String on the RX Line:

Arduino -- TX --> RX: PC
            |
            | --> RX: LCD Module

This string contains the machine position and her state. It will be parse and display on LCD, u can also choose display debug screen / work position / machine position with an Button on this device.

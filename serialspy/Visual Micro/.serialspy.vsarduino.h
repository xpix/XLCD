//Board = Arduino Duemilanove w/ ATmega328
#define __AVR_ATmega328P__
#define ARDUINO 105
#define F_CPU 16000000L
#define __AVR__
extern "C" void __cxa_pure_virtual() {;}

//
//
String getValue(String data, char separator, int index);
void parsePCCommand(String line);
void parseGrblLine(String line);
byte ReadButton();
void parse_command_line(String line);
void setinterval(String line);
void resetDevice();
void memoryfree();
void about();
void parse_status_line(String line);
void parse_state_line(String myBuffer);
void simpleThread_setup(getPositions);
boolean simpleThread_loop(getPositions);
void simpleThread_setup(getStates);
boolean simpleThread_loop(getStates);

#include "C:\Users\c5125630\Dropbox\arduino-1.0.5\hardware\arduino\variants\standard\pins_arduino.h" 
#include "C:\Users\c5125630\Dropbox\arduino-1.0.5\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\serialspy.ino"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\buttons_thread.ino"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\commands.ino"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\lcd_screens.ino"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\timebased_threads.ino"

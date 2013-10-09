/* 
	Editor: http://www.visualmicro.com
	        arduino debugger, visual micro +, free forum and wiki
	
	Hardware: Arduino Uno, Platform=avr, Package=arduino
*/

#define __AVR_ATmega328P__
#define _VMDEBUG 1
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

//
//
char* split( char* string, char* delimiter, int index );
void parsePCCommand(char* line);
void parseGrblLine(char* line);
void call_button(byte number);
void call_button_1();
void call_button_2();
void call_button_3();
void call_button_4();
void call_button_5();
void call_button_6();
void call_button_7();
void call_button_A();
void call_button_B();
void call_button_C();
void call_button_D();
void call_button_E();
void call_button_F();
void call_button_G();
byte ReadButton();
void parse_command_line(char* line);
void show_button();
void free_ram();
void learn_button();
void press_button(char* line);
void setinterval(char* line);
void setinterval_ms(int ms);
void resetDevice(int n);
void about();
void parse_status_line(char* line);
void parse_state_line(char* myBuffer);
void get_button_values();
int get_set_button_power(int button, int intvalue);
void EEPROMWriteInt(int p_address, int p_value);
unsigned int EEPROMReadInt(int p_address);
void simpleThread_setup(getPositions);
boolean simpleThread_loop(getPositions);
void simpleThread_setup(getStates);
boolean simpleThread_loop(getStates);
int freeRam ();

#include "C:\Users\xpix\Dropbox\arduino-1.0.5\hardware\arduino\variants\standard\pins_arduino.h" 
#include "C:\Users\xpix\Dropbox\arduino-1.0.5\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\xpix\Dropbox\GitHub\XStepperLCD\serialspy\serialspy.ino"
#include "C:\Users\xpix\Dropbox\GitHub\XStepperLCD\serialspy\LCDMenu.cpp"
#include "C:\Users\xpix\Dropbox\GitHub\XStepperLCD\serialspy\LCDMenu.h"
#include "C:\Users\xpix\Dropbox\GitHub\XStepperLCD\serialspy\buttons_thread.ino"
#include "C:\Users\xpix\Dropbox\GitHub\XStepperLCD\serialspy\config_buttons.ino"
#include "C:\Users\xpix\Dropbox\GitHub\XStepperLCD\serialspy\console.ino"
#include "C:\Users\xpix\Dropbox\GitHub\XStepperLCD\serialspy\lcd_menu.ino"
#include "C:\Users\xpix\Dropbox\GitHub\XStepperLCD\serialspy\lcd_screens.ino"
#include "C:\Users\xpix\Dropbox\GitHub\XStepperLCD\serialspy\persistance.ino"
#include "C:\Users\xpix\Dropbox\GitHub\XStepperLCD\serialspy\timebased_threads.ino"

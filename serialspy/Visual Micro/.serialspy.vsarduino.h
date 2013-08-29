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
void parse_command_line(String line);
void learn_button();
void press_button(String line);
void setinterval(String line);
void resetDevice(int n);
void memoryfree();
void about();
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
void menu_root(int disp);
void menu_mode(int n);
void menu_mode_set(int on);
void menu_about(int n);
void menu_interval(int n);
void _setInterval(int ms);
void menu_learn(int n);
void menu_exit(int n);
void parse_status_line(String line);
void parse_state_line(String myBuffer);
int get_set_button_power(int button, int intvalue);
void simpleThread_setup(getPositions);
boolean simpleThread_loop(getPositions);
void simpleThread_setup(getStates);
boolean simpleThread_loop(getStates);

#include "C:\Users\c5125630\Dropbox\arduino-1.0.5\hardware\arduino\variants\standard\pins_arduino.h" 
#include "C:\Users\c5125630\Dropbox\arduino-1.0.5\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\serialspy.ino"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\LCDMenu.cpp"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\LCDMenu.h"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\commands.ino"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\config_buttons.ino"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\lcd_menu.ino"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\lcd_screens.ino"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\persistance.ino"
#include "C:\Users\c5125630\Dropbox\GitHub\XStepperLCD\serialspy\timebased_threads.ino"

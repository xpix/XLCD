/* ===============================================
 * Project: XLCD
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

/* 
 * ===============================================
 * Includes
 * =============================================== 
 */
#include <simpleThread.h>
//#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include <MemoryFree.h>
#include <EEPROMEx.h>
#include <phi_interfaces.h>

// local libs
#include "LCDMenu.h"


/* Prototype on Arduino UNO or compatible
 * Frank (xpix) Herrmann / 07/2013
 *
 */

/* 
 * ===============================================
 * Defines
 * =============================================== 
 */

// Makros
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// XXX: 
// Test with SoftwareSerial, because AltSerial need Ports 8 and 9. 
// PCB Design use follow Connects
//    PC => ( Soft: 9,10 ) => XLCD => (UART: 0,1) => XStepper

// GRBL Serial connect pins
// SoftwareSerial (NewSoftSerial)
#define GRBL_RX     9
#define GRBL_TX     10 
// AltSoftSerial
//#define GRBL_RX        8
//#define GRBL_TX        9 

// LCD
//#define LCD_ADDR        0x27  // I2C LCD Address
#define LCD_4BIT
#define LCD_cols                          16
#define LCD_rows                          4

// Rotary Encoder
#define ENC_A           2   // Encoder interrupt pin
#define ENC_B           A3  // Encoder second pin
#define ENC_S           A2  // Encoder select pin

// Buttons
#define BUTTONS_A_ADC_PIN  A0    // A0 is the button ADC input A
#define BUTTONS_B_ADC_PIN  A1    // A1 is the button ADC input B

// Measure Power on when pressed button and note this value here
// use resistor network 680 Ohm
// Buttons:            0   1   2    3    4    5    6    7
int button_power[] = {30, 262, 415, 517, 589, 643, 685, 718}; // Power data for every pressed button

String			LINE;
String			CMDLINE;
char				GRBL_FIRST_CHAR;
char				PC_FIRST_CHAR;
volatile char	rotary_key;   // flag, follow button press
volatile byte  button_pressed;
boolean        proxymode = true;


/* 
 * ===============================================
 * Inits
 * =============================================== 
 */
// AltSoftSerial grblSerial;
SoftwareSerial grblSerial(GRBL_RX, GRBL_TX); // RX, TX

// Add Threads to refresh status informations from GRBL
#define _sT_cnt  _sT_cnt_2    // count of threads(?, $G)
simpleThread_init(_sT_cnt);   // init threads

// Describe threads
simpleThread_new_timebased_dynamic  (_sT_P1  , _sT_millis,  500, _sT_start , getPositions);	// get position info (?)
simpleThread_new_timebased_dynamic  (_sT_P2  , _sT_millis, 2000, _sT_start , getStates);	// get state info ($G) (not supported from UniversalGcodeSender)


// Rotary Encoder
#if defined(ENC_A)
	// Rotary Encoder
	char mapping1[]={'R','L'}; // This is a rotary encoder so it returns U for up and D for down on the dial.
	phi_rotary_encoders myEncoder(mapping1, ENC_A, ENC_B, 12);

	char mapping[]={'S'}; // This is a list of names for each button.
	byte pins[]={ENC_S}; // The digital pins connected to the 6 buttons.
	phi_button_groups myEncoderButton(mapping, pins, 1);
#endif

// Analog Buttons
#if defined(BUTTONS_A_ADC_PIN)
	byte keypad_type=Analog_keypad;
	char key_mapping_analog_a[]={'1','2','3','4','5','6','7'}; // This is an analog keypad A.
	byte pins_a[]={BUTTONS_A_ADC_PIN};
	phi_analog_keypads pad_a(key_mapping_analog_a, pins_a, button_power, 1, 7);

	char key_mapping_analog_b[]={'A','B','C','D','E','F','G'}; // This is an analog keypad B.
	byte pins_b[]={BUTTONS_B_ADC_PIN};
	phi_analog_keypads pad_b(key_mapping_analog_b, pins_b, button_power, 1, 7);
#endif

// All inits for LCD control
#if defined(LCD_4BIT)
   #include <LiquidCrystalFast.h>   // LCD
   LiquidCrystalFast myLCD(12, 11, 4, 5, 6, 7);
#elif defined(LCD_ADDR)
   #include <Wire.h>                // I2C Communication
   #include <LiquidCrystal_I2C.h>   // LCD over I2C
   // Set the pins on the I2C chip used for LCD connections:
   //                         addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
   LiquidCrystal_I2C myLCD(LCD_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
#endif

// LCD Menu
// look in lcd_menu.ino for build menus
LCDMenu menu(&myLCD, LCD_cols, LCD_rows);





/* 
 * ===============================================
 * Main
 * =============================================== 
 */

/*  ---------- Setup  ----------- */

void setup() 
{ 
   /* init threads */
   simpleThread_initSetup(_sT_cnt);

   myLCD.begin(16,4); // letter, row

   myLCD.setCursor(0,0); // letter, row
   myLCD.print(F("XLCD 0.1"));
   myLCD.setCursor(0,1); // letter, row
   myLCD.print(F("Connect ... "));

	menu_root(0); // init root menu but don't display

   // This is the serial connect to PC, we get some commands
   // but we can also print some additional information about this module
   // and the parser from Client program will ignore this  
   Serial.begin(9600);   // open serial to PC
   Serial.println(F("<XLCD 0.1>"));
   Serial.println(F("<All commands for XLCD start with a colon ':'>"));
   Serial.println(F("<Call help with ':?'>"));
  
   delay(1000);

   // open serial port to GRBL
   grblSerial.begin(9600); // open serial to grbl
   // reset grbl device (ctrl-X) for Universal Gcode Sender
   grblSerial.write(0x18);

   myLCD.clear();
}//SETUP



/*  ---------- Loop ----------- */
void loop() 
{ 
   // Jobs
	simpleThread_run(_sT_priority);

   // Read key from Rotary Encoder
	rotary_key = myEncoder.getKey();
	if(rotary_key == NO_KEY){
		rotary_key = myEncoderButton.getKey();
	}
	switch(rotary_key){
		case 'R':
			menu.up();
		break;
		case 'L':
			menu.down();
		break;
		case 'S':
			menu.mselect();
	}

   // try to read a button
	button_pressed=pad_a.getKey(); // Use phi_keypads object to access the keypad
	if (button_pressed == NO_KEY){
		button_pressed = pad_b.getKey(); // Use phi_keypads object to access the keypad
	}
	if(button_pressed != NO_KEY)
		call_button(button_pressed);
   

  // Get data from GRBL ==> PC
  if (grblSerial.available()) {
    char c = grblSerial.read();

    // save first char in a line
    if(LINE.length() == 0)
      GRBL_FIRST_CHAR = c;

    // dont send all line start with a '['
    // (UniversalGcodeSender dont like it)
    if(PC_FIRST_CHAR != '[')
      Serial.print(c);

    // wait for a complete line 
    // and parse it
    if(c == '\n'){
      parseGrblLine(LINE);
      LINE = "";
    } else {
      LINE.concat(c);
    }
  }

  // Get data from PC ==> GRBL
  if (Serial.available()) {
    char c = Serial.read();

    // save first char in a line
    if(CMDLINE.length() == 0)
      PC_FIRST_CHAR = c;

    // dont send all line start with a colon(XLCD Commands)
    if(PC_FIRST_CHAR != ':')
      grblSerial.print(c);

    // wait for a complete line
    // and parse it
    if(c == '\n'){
      parsePCCommand(CMDLINE);
      CMDLINE = "";
    } else {
      CMDLINE.concat(c);
    }
  }
}//LOOP


// ---------- Subroutines -----------

// String subtext = getValue(line, ',', 2);
// Split for strings, i.e. split at command an get the third value
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// Analyze every command (from PC => Xlcd) and choose an action
void parsePCCommand(String line){
  line.replace("\r","");

  // All commands with an ':' at start can control XLCD 
  if( line.indexOf(':') == 0 )  parse_command_line(line);
}

// Analyze every line and choose an action
void parseGrblLine(String line){
  line.replace("\r","");

  if( line.indexOf('<') == 0 )  parse_status_line(line);
  if( line.indexOf('[') == 0 )  parse_state_line(line);
}


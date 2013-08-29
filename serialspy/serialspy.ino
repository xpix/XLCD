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
#include <EEPROMEx.h>
#include <simpleThread.h>
#include <AltSoftSerial.h>
#include <MemoryFree.h>
#include <phi_interfaces.h>


//#include <SoftwareSerial.h>

/* Prototype on Arduino UNO or compatible
 * Frank (xpix) Herrmann / 07/2013
 *
 */

/* 
 * ===============================================
 * Defines
 * =============================================== 
 */

// XXX: 
// Test with SoftwareSerial, because AltSerial need Ports 8 and 9. 
// PCB Design use follow Connects
//    PC => ( Soft: 9,10 ) => XLCD => (UART: 0,1) => XStepper

// GRBL Serial connect pins
// SoftwareSerial
#define GRBL_RX         9
#define GRBL_TX         10 

// AltSoftSerial
#define GRBL_RX         8
#define GRBL_TX         9 

// LCD
//#define LCD_ADDR        0x27  // I2C LCD Address
#define LCD_4BIT
#define LCD_cols                          16
#define LCD_rows                          4


// Rotary Encoder
#define ENC_A           2   // Encoder interrupt pin
#define ENC_B           A3  // Encoder second pin
#define ENC_S           3  // Encoder select pin
volatile char rotary_key;   // flag, follow button press

// Buttons
#define BUTTONS_A_ADC_PIN  A0    // A0 is the button ADC input A
#define BUTTONS_B_ADC_PIN  A1    // A1 is the button ADC input B
#define BUTTONHYSTERESIS   10    // hysteresis for valid button sensing window
#define BUTTON_NONE        0     // no pressed state
#define BUTTONS_CNT        7     // how much buttons on one pin

// Measure Power on when pressed button and note this value here
// use resistor network 680 Ohm
byte buttonJustPressed  = false;         //this will be true after a ReadButtons() call if triggered
byte buttonJustReleased = false;         //this will be true after a ReadButtons() call if triggered
byte buttonWas          = BUTTON_NONE;   //used by ReadButtons() for detection of button events   

// Parser
String LINE;
String CMDLINE;
char GRBL_FIRST_CHAR;
char PC_FIRST_CHAR;

// LCD Menu --------------
// symbol to describe selected point
const char LCD_select_char = '>';
int LCD_offset =	-1;
int LCD_select =	0;
struct menuitem
{
	char name[28];
	void (*callback)(void);
};
// maximum entrys (10)
struct menuitem items[10];
// count entrys
int menucounter = 0;

// add menu entry [menutext, callback]
void menu_add_item(const char* text, void (*menu_callback)(void))
{
	int i = menucounter++;
	struct menuitem *item = &items[i];
	strncpy(item->name, text, sizeof(item->name));
	if(menu_callback)
	item->callback = menu_callback;
}


/* 
 * ===============================================
 * Inits
 * =============================================== 
 */
//SoftwareSerial grblSerial(GRBL_RX, GRBL_TX); // RX, TX

// Intialize menu tree

// Add Threads to refresh status informations from GRBL
#define _sT_cnt  _sT_cnt_3    // count of threads(?, $G)
simpleThread_init(_sT_cnt);   // init threads
simpleThread_new_timebased_dynamic  (_sT_P1  , _sT_millis,  200, _sT_start , drawMenu);	// draw lcd menu
simpleThread_new_timebased_dynamic  (_sT_P2  , _sT_millis,  500, _sT_start , getPositions);	// get position info (?)
simpleThread_new_timebased_dynamic  (_sT_P3  , _sT_millis, 2000, _sT_start , getStates);	// get state info ($G) (not supported from UniversalGcodeSender)

AltSoftSerial grblSerial;

// Rotary Encoder
#if defined(ENC_A)
	// Rotary Encoder
	char mapping1[]={'R','L'}; // This is a rotary encoder so it returns U for up and D for down on the dial.
	phi_rotary_encoders myEncoder(mapping1, ENC_A, ENC_B, 12);

   char mapping[]={'S'}; // This is a list of names for each button.
   byte pins[]={ENC_S}; // The digital pins connected to the 6 buttons.
   phi_button_groups myEncoderButton(mapping, pins, 1);
#endif

// All inits for LCD control
#if defined(LCD_4BIT)
   #include <LiquidCrystal.h>   // LCD
   LiquidCrystal myLCD(12, 11, 4, 5, 6, 7);
#elif defined(LCD_ADDR)
   #include <Wire.h>                // I2C Communication
   #include <LiquidCrystal_I2C.h>   // LCD over I2C
   // Set the pins on the I2C chip used for LCD connections:
   //                         addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
   LiquidCrystal_I2C myLCD(LCD_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
#endif


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

   myLCD.begin(LCD_cols, LCD_rows);

	// Add entrys to LCD Menu
   menu_add_item("1. menuentry", menu_1);
   menu_add_item("2. menuentry", menu_2);
   menu_add_item("3. menuentry", menu_1);
   menu_add_item("4. menuentry", menu_2);
   menu_add_item("About",			menu_about);
   menu_add_item("Exit",         menu_exit);

   // This is the serial connect to PC, we get some commands
   // but we can also print some additional information about this module
   // and the parser from Client program will ignore this
   Serial.begin(9600);   // open serial to PC
   Serial.println(F("<XLCD 0.1>"));
   Serial.println(F("<All commands for XLCD start with a colon ':'>"));
   Serial.println(F("<Call help with ':?'>"));

   // Old LCD Screens
   myLCD.begin(16,4); // letter, row

   myLCD.setCursor(0,0); // letter, row
   myLCD.print(F("XLCD 0.1"));
   myLCD.setCursor(0,1); // letter, row
   myLCD.print(F("Connect ... "));
   
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
			LCD_select++;
			menu_display(LCD_offset++); // one down
			break;
		case 'L':
			LCD_select--;
			menu_display(LCD_offset--); // one down
			break;
		case 'S':
			menu_select(LCD_select);
			break;
	}

   // try to read a button
   byte button = ReadButton();
   if(button && buttonJustPressed){
      call_button(button);
   }

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

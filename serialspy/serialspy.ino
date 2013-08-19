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
#include <AltSoftSerial.h>
#include <Encoder.h>
#include <LCD.h>

#include <MemoryFree.h>
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
// #define GRBL_RX        9
// #define GRBL_TX        10 
// AltSoftSerial
#define GRBL_RX        8
#define GRBL_TX        9 

// LCD
//#define LCD_ADDR        0x27  // I2C LCD Address
#define LCD_4BIT
#define LCD_cols                          16
#define LCD_rows                          4

// Rotary Encoder
//#define ENC_A           2   // Encoder interrupt pin
//#define ENC_B           A3  // Encoder second pin
//#define ENC_S           A4  // Encoder select pin

// Buttons
#define BUTTONS_A_ADC_PIN  A0    // A0 is the button ADC input A
#define BUTTONS_B_ADC_PIN  A1    // A1 is the button ADC input B
#define BUTTONHYSTERESIS   10    // hysteresis for valid button sensing window
#define BUTTON_NONE        0     // no pressed state
// Measure Power on when pressed button and note this value here
// use resistor network 680 Ohm
// Buttons:            0   1   2    3    4    5    6    7
int button_power[] = {30, 262, 415, 517, 589, 643, 685, 718}; // Power data for every pressed button
byte buttonJustPressed  = false;         //this will be true after a ReadButtons() call if triggered
byte buttonJustReleased = false;         //this will be true after a ReadButtons() call if triggered
byte buttonWas          = BUTTON_NONE;   //used by ReadButtons() for detection of button events   

String LINE;
String CMDLINE;
char GRBL_FIRST_CHAR;
char PC_FIRST_CHAR;

/* 
 * ===============================================
 * Inits
 * =============================================== 
 */
//SoftwareSerial mySerial(GRBL_RX, GRBL_TX); // RX, TX

// Add Threads to refresh status informations from GRBL
#define _sT_cnt  _sT_cnt_2    // count of threads(?, $G)
simpleThread_init(_sT_cnt);   // init threads

// Describe threads
simpleThread_new_timebased_dynamic  (_sT_P1  , _sT_millis, 5000, _sT_stop , getPositions);	// get position info (?)
simpleThread_new_timebased_dynamic  (_sT_P2  , _sT_millis,10000, _sT_stop , getStates);	// get state info ($G) (not supported from UniversalGcodeSender)

AltSoftSerial mySerial;

// Rotary Encoder
#if defined(ENC_A)
  Encoder myEncoder(ENC_A, ENC_B);
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

  // This is the serial connect to PC, we get some commands
  // but we can also print some additional information about this module
  // and the parser from Client program will ignore this  
  Serial.begin(9600);   // open serial to PC
  Serial.println(F("<XLCD 0.1>"));
  Serial.println(F("<All commands for XLCD start with a colon ':'>"));
  Serial.println(F("<Call help with ':?'>"));
  
  delay(1000);

  mySerial.begin(9600); // open serial to grbl
  myLCD.clear();
}//SETUP



/*  ---------- Loop ----------- */
void loop() 
{ 
  // Jobs
   simpleThread_run(_sT_priority);

  // try to read a button
   byte button = ReadButton();
   if(button && (buttonJustPressed || buttonJustReleased) ){
      // action ...
	Serial.print("Button: ");
	Serial.println(button);
	Serial.print("pressed: ");
	Serial.println(buttonJustPressed);
	Serial.print("released: ");
	Serial.println(buttonJustReleased);
   }

  // Get data from GRBL ==> PC
  if (mySerial.available()) {
    char c = mySerial.read();

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
      mySerial.print(c);

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


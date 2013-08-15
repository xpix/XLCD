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

//#include <MemoryFree.h>
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
// Test with SoftwareSerial, cuz AltSerial need Ports 8 and 9. 
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
//#define ENC_A           2   // Encoder interupt pin
//#define ENC_B           A3  // Encoder second pin
//#define ENC_S           A4  // Encoder select pin

String LINE;
String CMDLINE;
boolean XMENU = false;

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
simpleThread_new_timebased_static  (_sT_P1  , _sT_millis, 1000, _sT_start  , Loop_1); // get position info (?)
simpleThread_new_timebased_static  (_sT_P2  , _sT_millis, 2000, _sT_start  , Loop_2); // get state info ($G)

AltSoftSerial mySerial;

// Rotary Encoder
#if defined(ENC_A)
  Encoder myEncoder(ENC_A, ENC_B);
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

  myLCD.setCursor(0,0); // letter, row
  myLCD.print("XLCD 0.1");
  myLCD.setCursor(0,1); // letter, row
  myLCD.print("Connect ... ");

  // This is the serial connect to PC, we get some commands
  // but we can also print some additional information about this module
  // and the parser from Clientprogram will ignore this  
  Serial.begin(57600);   // open serial to PC
  Serial.println("XLCD 0.1");
  Serial.println("All commands for XLCD start with a colon ':'");
  Serial.println("Call help with ':?'");
  
  delay(1000);

  mySerial.begin(9600); // open serial to grbl
}//SETUP



/*  ---------- Loop ----------- */
void loop() 
{ 

  /* calls the threads */
  simpleThread_run(_sT_priority);

  char c;
  // Get data from PC and send to GRBL
  if (Serial.available()) {
    c = Serial.read();
    // check if a xlcd command
    if(CMDLINE.length() == 0 && c == ':'){
      XMENU = true;
    }
    // send only if not a XLCD command
    if(!XMENU)
      mySerial.print(c);

    // end of row, parse command
    if(c == '\n'){
      parsePCCommand(CMDLINE);
      CMDLINE = "";
      XMENU = false;
    } else {
      CMDLINE.concat(c);
    }
  }
  // Get data from GRBL and send to PC
  if (mySerial.available()) {
    c = mySerial.read();
    if(c == '\n'){
      parseGrblLine(LINE);
      LINE = "";
    } else {
      LINE.concat(c);
    }
    Serial.print(c);
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
  Serial.println("ok");
}

// Analyze every line and choose an action
void parseGrblLine(String line){
  line.replace("\r","");

  if( line.indexOf('<') == 0 )  return parse_status_line(line);
  if( line.indexOf('[') == 0 )  return parse_state_line(line);
}


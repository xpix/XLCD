/* ===============================================
 * Project: XLCD
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

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
#define GRBL_RX        9
#define GRBL_TX        10 
// AltSoftSerial
#define GRBL_RX        8
#define GRBL_TX        9 

// LCD
//#define LCD_ADDR        0x27  // I2C LCD Address
#define LCD_4BIT
#define _LCDMenuLib_cols                          16
#define _LCDMenuLib_rows                          4

#define _LCDMenuLib_PIN_rs                        12 
#define _LCDMenuLib_PIN_e                         11
#define _LCDMenuLib_PIN_dat4                      4
#define _LCDMenuLib_PIN_dat5                      5
#define _LCDMenuLib_PIN_dat6                      6
#define _LCDMenuLib_PIN_dat7                      7 

#define _LCDMenuLib_ENABLE_INIT_SCREEN            1
#define _LCDMenuLib_INIT_SCREEN_TIME              10000

#define _LCDMenuLib_cnt                           0


// Rotary Encoder
#define ENC_A           2   // Encoder interupt pin
#define ENC_B           A3  // Encoder second pin
#define ENC_S           A4  // Encoder select pin

#define _BUTTON_press_time                 200 //ms
#define _BUTTON_PIN                        0  
#define _BUTTON_enter_min                  850     // Button Enter
#define _BUTTON_enter_max                  920  
#define _BUTTON_back_min                   950     // Button Back
#define _BUTTON_back_max                   1020  
#define _BUTTON_up_min                     520     // Button Up
#define _BUTTON_up_max                     590  
#define _BUTTON_down_min                   700     // Button Down
#define _BUTTON_down_max                   770      
#define _BUTTON_left_min                   430     // Button Left
#define _BUTTON_left_max                   500  
#define _BUTTON_right_min                  610     // Button Right
#define _BUTTON_right_max                  680     

unsigned long g_button_press_time = 0;


String LINE;
String CMDLINE;
//#include <MemoryFree.h>

//#include <SoftwareSerial.h>      // listen on TX Line from GRBL to PC
//SoftwareSerial mySerial(GRBL_RX, GRBL_TX); // RX, TX

#include <AltSoftSerial.h>
AltSoftSerial mySerial;

// Rotary Encoder
#if defined(ENC_A)
  #include <Encoder.h>
  Encoder myEncoder(ENC_A, ENC_B);
#endif

#if defined(LCD_4BIT)
   #include <LiquidCrystal.h>   // LCD
   #include <LCDMenuLib.h>      // menu over LCD

   LCDMenuLib_init(_LCDMenuLib_cnt);
   LCDMenuLib_element(0  , Item         , Item_1        , "Information"        , FUNC_information);
   LCDMenuLib_element(1  , Item         , Item_2        , "Zeit Info"          , FUNC_timer_info);
   LCDMenuLib_element(2  , Item         , Item_3        , "Einstellungen"      , FUNC);
   LCDMenuLib_element(3  , Item_3       , Item_3_1      , "Werte veraendern"   , FUNC);
   LCDMenuLib_element(4  , Item_3       , Item_3_2      , "Irgendwas"          , FUNC);
   LCDMenuLib_element(5  , Item         , Item_4        , "Programme"          , FUNC);
   LCDMenuLib_element(6  , Item_4       , Item_4_1      , "Programme 1"        , FUNC);
   LCDMenuLib_element(7  , Item_4_1     , Item_4_1_1    , "P1 Starten"         , FUNC);
   LCDMenuLib_element(8  , Item_4_1     , Item_4_1_2    , "Einstellungen"      , FUNC);
   LCDMenuLib_element(9  , Item_4_1_2   , Item_4_1_2_1  , "extra warm"         , FUNC);
   LCDMenuLib_element(10 , Item_4_1_2   , Item_4_1_2_2  , "extra lang"         , FUNC);
   LCDMenuLib_element(11 , Item_4       , Item_4_2      , "Programm 2"         , FUNC_p2);
   LCDMenuLib_element(12 , Item_4       , Item_4_3      , "Programm Mix"       , FUNC);
   LCDMenuLib_element(13 , Item_4       , Item_4_4      , "Programm Hemden"    , FUNC);
   LCDMenuLib_element(14 , Item_4       , Item_4_5      , "Programm Schnell"   , FUNC);
   LCDMenuLib_element(15 , Item_4       , Item_4_6      , "Einstellungen"      , FUNC);
   LCDMenuLib_element(16 , Item_4_6     , Item_4_6_1    , "Irgendwas"          , FUNC);
   LCDMenuLib_createMenu(_LCDMenuLib_cnt);

   LiquidCrystal myLCD(12, 11, 4, 5, 6, 7);
#elif defined(LCD_ADDR)
   #include <Wire.h>                // I2C Communication
   #include <LCD.h>
   #include <LiquidCrystal_I2C.h>   // LCD over I2C
   // #include <LCDMenuLib_I2C.h>   // Menu over I2C LCD
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

  pinMode(GRBL_RX, INPUT);
  pinMode(GRBL_TX, OUTPUT);

  LCDMenuLib_setup(_LCDMenuLib_cnt);

  // This is the serial connect to PC, we get some commands
  // but we can also print some additional information about this module
  // and the parser from Clientprg will ignore this  
  Serial.begin(57600);   // open serial to PC
  Serial.println("XLCD 0.1");
  Serial.println("All commands for XLCD start with a colon ':'");
  
  delay(1000);

  mySerial.begin(9600); // open serial to grbl
  // myLCD.clear();
}//SETUP


/*  ---------- Loop ----------- */

void loop() 
{ 
  LCDMenuLib_loop();  

  char c;
  if (Serial.available()) {

    c = Serial.read();
    mySerial.print(c);
    if(c == '\n'){
      parseCommand(CMDLINE);
      CMDLINE = "";
    } else {
      CMDLINE.concat(c);
    }
  }
  if (mySerial.available()) {
    c = mySerial.read();
    if(c == '\n'){
      parseLine(LINE);
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
void parseCommand(String line){
  line.replace("\r","");

  // All commands with an ':' at start can control XLCD 
  if( line.indexOf(':') == 0 )  parse_command_line(line);
}

void parse_command_line(String line)
{
  line.replace("\r","");
  line.replace(":","");

  // do something

}

// Analyze every line and choose an action
void parseLine(String line){
  line.replace("\r","");

  if( line.indexOf('<') == 0 )  parse_status_line(line);
  if( line.indexOf('[') == 0 )  parse_state_line(line);
}

void parse_status_line(String line)
{
  // <Idle,MPos:5.529,0.560,7.000,WPos:1.529,-5.440,-0.000>
  // State ..
  String state = getValue(getValue(line, ',', 0), '<', 1);  

  // Machine position ...
  String machinepos_x = getValue(getValue(line, ',', 1), ':', 1);
  String machinepos_y = getValue(line, ',', 2);
  String machinepos_z = getValue(line, ',', 3);

  // Working position ...
  String workingpos_x = getValue(getValue(line, ',', 4), ':', 1);
  String workingpos_y = getValue(line, ',', 5);
  String workingpos_z = getValue(getValue(line, ',', 6), '>', 0);

  // return if not state
  if(workingpos_z.length() == 0){
    return;
  } 

  // Display on LCD ... 
  // lcd screen
  // |--------------|
  // State  Z:555.529
  // 000.000  000.000

  // XXX: to use a switch to display work position or other things :)
  // myLCD.clear();
  // Display State and Machine position in first 2 rows
  myLCD.setCursor(0,0); // letter, row
  myLCD.print(state + "            ");
  myLCD.setCursor((_LCDMenuLib_cols-machinepos_z.length()),0);
  myLCD.print(machinepos_z);

  myLCD.setCursor(0,1);
  myLCD.print(machinepos_x);
  myLCD.setCursor((_LCDMenuLib_cols-machinepos_y.length()),1);
  myLCD.print(machinepos_y);
}

// send every second the command $G
void parse_state_line(String myBuffer)
{
  // parse myBuffer
  myBuffer.replace("[","");
  myBuffer.replace("]","");
  //             mm                   TNr Feed
  // [G0 G54 G17 G21 G90 G94 M0 M5 M9 T0 F500.000]
  String move = getValue(myBuffer, ' ', 0);  
  if(move == "G")  move = "RAP";
  if(move == "G0") move = "RAP";
  if(move == "G1") move = "LIN";
  if(move == "G2") move = "CW ";
  if(move == "G3") move = "CCW";

  String plane = getValue(myBuffer, ' ', 2);  
  if(plane == "G17") plane = "XY";
  if(plane == "G18") plane = "ZX";
  if(plane == "G19") plane = "YZ";

  String feedrate = getValue(myBuffer, ' ', 10);  
  feedrate.replace(".000","");

  String toolnr = getValue(myBuffer, ' ', 9);  
  String spindle = (getValue(myBuffer, ' ', 7) == "M5" ? "1" : "0");  

  String mode = (getValue(myBuffer, ' ', 3) == "G21" ? "MM" : "IN");  
  String flow = getValue(myBuffer, ' ', 6);  

  // Display on LCD ... 
  // |--------------|
  // S1 T1 F1000
  // MM LIN XY M1

  // Spindle
  myLCD.setCursor(0,2); //third row
  myLCD.print("S" + spindle);

  // Tool
  myLCD.setCursor(3,2);
  myLCD.print(toolnr);

  // Feed
  myLCD.setCursor(6,2); //third row
  myLCD.print(feedrate);


  // last row

  // mm or inch
  myLCD.setCursor(0,3);
  myLCD.print(mode);

  // Move mode
  myLCD.setCursor(3,3);
  myLCD.print(move);

  // Plane
  myLCD.setCursor(7,3);
  myLCD.print(plane);

  // Flow
  myLCD.setCursor(10,3);
  myLCD.print(flow);

}


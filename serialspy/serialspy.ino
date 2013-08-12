/* Prototype on Arduino UNO or compatible
 * Frank (xpix) Herrmann / 07/2013
 *
 * NOTE! - It's very important to use pullups on the SDA & SCL lines!

  The buttons are numbered in follow schema with pins on buttons connector:
  A = 1(3), 2(5), 3(7), 4(9), 5(11), 6(13), 7(15), 8(15), 9(17)
  B = 11(6), 12(8), 13(10), 14(12), 15(14), 16(16), 17(18), 18(20)

  if you connect a button on Pin12 and GND the onPress 
  you will get button number 14 if pressed.
 */


//
// ---------- DEFINES -----------
//
// GRBL Serial connect pins
#define GRBL_RX        8
#define GRBL_TX        9 

// LCD
//#define LCD_ADDR        0x27  // I2C LCD Address
#define LCD_4BIT
#define LCD_LETTERS     16    // how much letters in a row
#define LCD_ROWS        4     // how much rows

// Rotary Encoder
//#define ENC_A           2   // Encoder interupt pin
//#define ENC_B           A3  // Encoder second pin
//#define ENC_S           A4  // Encoder select pin

// Buttons
#define BUTTONS_A_ADC_PIN  A0    // A0 is the button ADC input A
#define BUTTONS_B_ADC_PIN  A1    // A1 is the button ADC input B
#define BUTTONHYSTERESIS   10    // hysteresis for valid button sensing window
#define BUTTON_NONE        0     // no pressed state
// Measure Power on when pressed button and note this value here  
// Buttons:            0   1   2    3    4    5    6    7
int button_power[] = {32, 64, 96, 128, 256, 512, 868, 999}; // Power data for every pressed button


//
// ---------- INCLUDES -----------
//
//#include <MemoryFree.h>
//#include <SoftwareSerial.h>      // listen on TX Line from GRBL to PC
#include <AltSoftSerial.h>
#include <Timer.h>

// Rotary Encoder
#if defined(ENC_A)
  #include <Encoder.h>
  Encoder myEncoder(ENC_A, ENC_B);
#endif


// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
#include <LCD.h>
#if defined(LCD_4BIT)
  #include <LiquidCrystal.h>   // LCD over I2C
  LiquidCrystal myLCD(12, 11, 4, 5, 6, 7);
#elif defined(LCD_ADDR)
  #include <Wire.h>                // I2C Communication
  #include <LiquidCrystal_I2C.h>   // LCD over I2C
  LiquidCrystal_I2C myLCD(LCD_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
#endif

//Timer myTimer;
//SoftwareSerial mySerial(GRBL_RX, GRBL_TX); // RX, TX
AltSoftSerial mySerial;

String LINE;


//
// ---------- ROUTINES -----------
//


/*--------------------------------------------------------------------------------------
  onPress(int button)
  Call some action if your button pressed, 
--------------------------------------------------------------------------------------*/
void onPress(int button){
  switch( button )
   {
      case 1: // First button
      {
        // do something
        break;
      }
      case 2: // Second button
      {
        // do something
        break;
      }
      case 14: // some button
      {
        // do something
        break;
      }
      // .... and so on :)
      default: // First button
      {
         break;
      }
   };  

}

// ---------- Setup  -----------

void setup() 
{ 

  pinMode(GRBL_RX, INPUT);
  pinMode(GRBL_TX, OUTPUT);

  myLCD.begin(LCD_LETTERS,LCD_ROWS); 
  myLCD.clear();
  myLCD.setCursor(0, 0);
  myLCD.print("XstepperLCD 0.1");
   
  // This is the serial connect to PC, we get some commands
  // but we can also print some additional information about this module
  // and the parser from Clientprg will ignore this  
  Serial.begin(57600);   // open serial to PC
  Serial.println("XStepperLCD 0.1");
  
  delay(1000);

  mySerial.begin(9600); // open serial to grbl
  myLCD.clear();
}//SETUP


// ---------- Loop  -----------

void loop() 
{ 
  // Update events for timer lib
  // myTimer.update();

  // on change buttons
  if(byte button = ReadButtons()){
    onPress(button);
  }

  char c;
  if (Serial.available()) {
    c = Serial.read();
    mySerial.print(c);
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

// Analyze every line and choose an action
void parseLine(String line){
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

  // XXX: to use a switch to display workpos or other things :)
  // myLCD.clear();
  // Display State and Machineposition in first 2 rows
  myLCD.setCursor(0,0); // letter, row
  myLCD.print(state + "            ");
  myLCD.setCursor((LCD_LETTERS-machinepos_z.length()),0);
  myLCD.print(machinepos_z);

  myLCD.setCursor(0,1);
  myLCD.print(machinepos_x);
  myLCD.setCursor((LCD_LETTERS-machinepos_y.length()),1);
  myLCD.print(machinepos_y);
}

// send every second the command $G
void parse_state_line(String myBuffer)
{
  // parse myBuffer
  //             mm                   TNr Feed
  // [G0 G54 G17 G21 G90 G94 M0 M5 M9 T0 F500.000]
  String move = getValue(myBuffer, ' ', 0);  
  move.replace("[","");
  move.replace("]","");
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

/*--------------------------------------------------------------------------------------
  ReadButtons()
  Detect the button pressed and return the value
  Uses global values buttonWas, buttonJustPressed, buttonJustReleased.
--------------------------------------------------------------------------------------*/
byte ReadButtons()
{
   // return no button pressed if the below checks don't write to btn
   byte button = BUTTON_NONE;   
   
   //read the button ADC pin voltage first on A then on B
   char buttonrow = 'A';
   unsigned int buttonVoltage = analogRead( BUTTONS_A_ADC_PIN );
   if(buttonVoltage <= 5 + BUTTONHYSTERESIS){
      buttonrow = 'B';
      buttonVoltage = analogRead( BUTTONS_B_ADC_PIN );
   }
   if(buttonVoltage <= 5 + BUTTONHYSTERESIS){
      return button;
   }

   // test all values to discover button
   for (int i = 0; i < 7 ; i = i + 1) {
      if( buttonVoltage < ( button_power[i] + BUTTONHYSTERESIS ) )
      {
         button = i;
      }
   }   

   if( buttonrow == 'B'){
      button = button + 10;
   }
   
   return( button );
}



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
// LCD
//#define LCD_ADDR        0x27  // I2C LCD Address
#define LCD_4BIT
#define LCD_LETTERS     20    // how much letters in a row
#define LCD_ROWS        4     // how much rows

// Rotary Encoder
#define ENC_A           2   // Encoder interupt pin
#define ENC_B           A3  // Encoder second pin
#define ENC_S           A4  // Encoder select pin

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
#include <SoftwareSerial.h>      // listen on TX Line from GRBL to PC
#include <Timer.h>

// Rotary Encoder
#if defined(ENC_A)
  #include <Encoder.h>
  Encoder myEncoder(ENC_A, ENC_B);
#endif


// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
#if defined(LCD_4BIT)
  #include <LiquidCrystal.h>   // LCD over I2C
  LiquidCrystal myLCD(12, 8, 4, 5, 6, 7);
#elif defined(LCD_ADDR)
  #include <Wire.h>                // I2C Communication
  #include <LCD.h>
  #include <LiquidCrystal_I2C.h>   // LCD over I2C
  LiquidCrystal_I2C myLCD(LCD_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
#endif

Timer myTimer;
SoftwareSerial mySerial(10, 11); // RX, TX

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

  myLCD.begin(LCD_LETTERS,LCD_ROWS); 
  myLCD.backlight();
  myLCD.clear();
  myLCD.setCursor(0, 0);
  myLCD.print("XstepperLCD 0.1");
  myLCD.clear();

  // Register some events
  // getParserState get  all of the active gcode modes 
  // that the parser will interpret any incoming command
  // and display on the bottom side from LCD
  int tickEvent = myTimer.every(2000, getState);

   
  // This is the serial connect to PC, we get some commands
  // but we can also print some additional information about this module
  // and the parser from Clientprg will ignore this  
  Serial.begin(9600);   // open serial to PC
  Serial.println("XStepperLCD 0.1");
  
  delay(1000);

  mySerial.begin(9600); // open serial to grbl
}//SETUP


// ---------- Loop  -----------

void loop() 
{ 
  // Update events for timer lib
  t.update();

  // on change buttons
  if(byte button = ReadButtons()){
    onPress(button);
  }
  
  // Try to read from serial PC line and send
  // this to grbl serial line
  String buffer = "";
  while(Serial.available() > 0) {
      char character = Serial.read();
      if(character == '\n'){
        Serial.println("Read: " + buffer);

        // try to parse this line
        // and display on LCD
        parse_status_line(buffer);

        // send characters to grbl
        // as proxy, or change some 
        // commands and send to grbl
        mySerial.println(buffer);
        
        buffer = "";
      } else {
        buffer.concat(character);
      }
  }

  // Get infos from grbl line 
  // and send back to PC line
  while(mySerial.available() > 0){
    Serial.write(mySerial.read());
  }

  delay(100);
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

void parse_status_line( String line )
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
void getState()
{
  String myBuffer = "";
  mySerial.println("$G");
  while(mySerial.available() > 0) {
      char character = mySerial.read();
      if(character == '\n'){
        // parse myBuffer
        // [G0 G54 G17 G21 G90 G94 M0 M5 M9 T0 F500.000]

        String feedrate = getValue(getValue(myBuffer, ' ', 10), ']', 0);  
        feedrate.replace("F","");
        feedrate.replace(".000","");
        feedrate = "F:" + feedrate; 

        String toolnr = "T:" + getValue(myBuffer, ' ', 9);  
        int spindle = (getValue(myBuffer, ' ', 7) == "M5" ? 0 : 1);  
      
        // Display on LCD ... 
        // |--------------|
        // Sp:off F:500 T:1

        // Spindle
        myLCD.setCursor(0,2); //third row
        myLCD.print("Sp:" + spindle ? "on " : "off");

        // Feed
        myLCD.setCursor(6,2); //third row
        myLCD.print(feedrate);

        // Tool
        myLCD.setCursor((LCD_LETTERS-toolnr.length()),2);
        myLCD.print(toolnr);

        // maybe add inch/mm display ... 

        myBuffer = "";
      } else {
        myBuffer.concat(character);
      }
  }
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


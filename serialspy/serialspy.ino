/* Prototype on Arduino UNO or compatible
 * Frank (xpix) Herrmann / 07/2013
 *
 *    Arduino RX from GRBL/XStepper Board TX with 220ohm Resistor
 *    Arduino GND from GRBL/XStepper Board 
 *    Arduino A4 SDA  to I2C LCD Board 
 *    Arduino A5 SCL  to I2C LCD Board 
 *    Arduino VCC to  XLCD Board 
 *    Arduino GND to  XLCD Board 
 *
 * NOTE! - It's very important to use pullups on the SDA & SCL lines!
 * LiquidCrystal_I2C lib was modified for ATtiny - on Playground with TinyWireM lib.
 * TinyWireM USAGE & CREDITS: - see TinyWireM.h
 */

//#define DEBUG
//#include <MemoryFree.h>
#include <Wire.h>                // I2C Communication (Attiny must use TinyWireM
#include <LCD.h>
#include <LiquidCrystal_I2C.h>   // LCD over I2C
#include <SoftwareSerial.h>      // listen on TX Line from GRBL to PC
#include <Timer.h>

#define BUTTON          1     // Button to control

#define LCD_ADDR        0x27  // I2C LCD Address
#define LCD_LETTERS  20
#define LCD_ROWS      4
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(LCD_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

Timer t;
SoftwareSerial mySerial(10, 11); // RX, TX

//Button to change menu .. not realized yet
//menu: 
// m = display machine position
// w = display work position
// d = display debug display
char menus[4] = "mwd";
int mchange = 0;
char option = menus[0]; // has default option
int  menusize = (sizeof(menus)/sizeof(char *)); //array size

void setup() 
{ 

  lcd.begin(LCD_LETTERS,LCD_ROWS); 
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("XstepperLCD 0.1");
  lcd.clear();

  // Register some events
  // getParserState get  all of the active gcode modes 
  // that the parser will interpret any incoming command
  // and display on the bottom side from LCD
  int tickEvent = t.every(2000, getParserState);
  
  Serial.begin(9600);   // open serial to PC
  Serial.println("XStepperLCD 0.1");
  Serial.print("Option: ");
  Serial.println(option);
  
  delay(1000);

  mySerial.begin(9600); // open serial to grbl
}//SETUP



void loop() 
{ 
  // Update events for timer lib
  t.update();

  // Try to read from serial PC line and send
  // this to grbl serial line
  String buffer = "";
  while(Serial.available() > 0) {
      char character = Serial.read();
      if(character == '\n'){
        Serial.println("Read: " + buffer);

        // try to parse this line
        // and display on LCD
        parse_line(buffer);

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

void parse_line( String line )
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
  // lcd.clear();
   if(option == 'm'){
     // Display State and Machineposition in first 2 rows
     lcd.setCursor(0,0); // letter, row
     lcd.print(state + "            ");
     lcd.setCursor((LCD_LETTERS-machinepos_z.length()),0);
     lcd.print(machinepos_z);

     lcd.setCursor(0,1);
     lcd.print(machinepos_x);
     lcd.setCursor((LCD_LETTERS-machinepos_y.length()),1);
     lcd.print(machinepos_y);
   }
   if(option == 'w'){
     // Display State and Workeposition in first 2 rows
     lcd.setCursor(0,0); // letter, row
     lcd.print(state + "            ");
     lcd.setCursor((LCD_LETTERS-workingpos_z.length()),0);
     lcd.print(workingpos_z);

     lcd.setCursor(0,1);
     lcd.print(workingpos_x);
     lcd.setCursor((LCD_LETTERS-workingpos_y.length()),1);
     lcd.print(workingpos_y);
   }
   if(option == 'd'){
      // Debug Infos
      lcd.print("Debugscreen");
   }
}


// send every second the command $G
void getParserState()
{
  String myBuffer = "";
  mySerial.println("$G");
  while(mySerial.available() > 0) {
      char character = mySerial.read();
      if(character == '\n'){
        // parse myBuffer
        // [G0 G54 G17 G21 G90 G94 M0 M5 M9 T0 F500.000]

        // State ..
        String feedrate = getValue(getValue(myBuffer, ' ', 10), ']', 0);  
        feedrate.replace("F","");
        feedrate.replace(".000","");
        String toolnr = getValue(myBuffer, ' ', 9);  
        int spindle = (getValue(myBuffer, ' ', 7) == "M5" ? 0 : 1);  
      
        // Spindle
        lcd.setCursor(0,2); //third row
        lcd.print("Sp:" + spindle ? "on" : "off");

        // Feed
        lcd.setCursor(7,2); //third row
        lcd.print("Feed:" + feedrate);

        // Tool
        lcd.setCursor(15,2); //third row
        lcd.print(toolnr);

        // maybe add inch/mm display ... 

        myBuffer = "";
      } else {
        myBuffer.concat(character);
      }
  }
}

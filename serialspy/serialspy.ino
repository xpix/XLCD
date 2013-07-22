/* ATtiny85 as an Serial Monitor to display information on LCD Display
 * SETUP:
 *
 *    (1) ATtiny D5 = RESET                            (8) ATtiny Pin -- = VCC (2.7-5.5V)
 *    (2) ATtiny D3 = TX (serial not use!)             (7) ATtiny D2 = SCK (I2C LCD)
 *    (3) ATtiny D4 = RX (serial monitor)              (6) ATtiny D1 = switch to control
 *    (4) ATtiny -- = GND                              (5) ATtiny D0 = SDA (I2C LCD) 
 *
 * NOTE! - It's very important to use pullups on the SDA & SCL lines!
 * LiquidCrystal_I2C lib was modified for ATtiny - on Playground with TinyWireM lib.
 * TinyWireM USAGE & CREDITS: - see TinyWireM.h
 */

//#define DEBUG
#include <MemoryFree.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, -1); // RX, TX

#define LCD_ADDR        0x21  // I2C LCD Address
#define BUTTON          1     // Button to control

LiquidCrystal_I2C lcd(LCD_ADDR,16,2); // 16 x 2 Display

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
  lcd.init(); 
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);

  Serial.begin(9600);
  Serial.println("XStepperLCD 0.1");

  lcd.print("XstepperLCD 0.1");
  Serial.println("Fertig");
  Serial.print("Option: ");Serial.println(option);
  delay(2000);
  lcd.clear();
  mySerial.begin(9600);
} 



void loop() 
{ 
  String buffer = "";
  char character;

  while(mySerial.available() > 0) {
      character = mySerial.read();
      if(character == '\n'){
        Serial.println("Read: " + buffer);
        parse_line(buffer);
        buffer = "";
      } else {
        buffer.concat(character);
      }
  }
  delay(50);
}//LOOP

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
  String workingpos_z = getValue(line, ',', 6);

  // return if not state
  if(workingpos_z.length() == 0){
    return;
  } 

  // Display on LCD ... 
  // lcd screen
  // |--------------|
  // State    X:5.529
  // Y:0.000  Z:0.000

  // XXX: to use a switch to display workpos or other things :)
  //lcd.clear();
   if(option == 'm'){
     lcd.setCursor(0,0); // letter, row
     lcd.print(state);
     lcd.setCursor(9,0);
     lcd.print(machinepos_x);
     lcd.setCursor(0,1);
     lcd.print(machinepos_y);
     lcd.setCursor(9,1);
     lcd.print(machinepos_z);
   }
   if(option == 'w'){
      lcd.setCursor(0,0); // letter, row
      lcd.print(state);
      lcd.setCursor(9,0);
      lcd.print(workingpos_x);
      lcd.setCursor(0,1);
      lcd.print(workingpos_y);
      lcd.setCursor(9,1);
      lcd.print(workingpos_z);
   }
   if(option == 'd'){
      // Debug Infos
      lcd.print("Debugscreen");
   }
   delay(20);
}

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
#include <TinyWireM.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR        0x27  // I2C LCD Address
#define BUTTON          1     // Button to control

LiquidCrystal_I2C lcd(LCD_ADDR,16,2); // 16 x 2 Display

int mchange = 0;
//menu: 
// m = display machine position
// w = display work position
// d = display debug display
char menus[4] = "mwd";
char option = menus[0]; // has default option
int  menusize = (sizeof(menus)/sizeof(char *)); //array size

void setup() 
{ 
  TinyWireM.begin();
  Serial.begin(9600);

  pinMode(BUTTON, INPUT);

  lcd.init(); 
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("XstepperLCD 0.1");
  delay(2000);
} 

void loop() 
{ 
  int buttonState = digitalRead(BUTTON);
  // Button pressed
  if(buttonState == HIGH){
      mchange++;
      if(mchange < menusize){
         option = menus[mchange]; // one option higer
      } else {
         mchange = 0;
         option = menus[mchange]; // first option
      }
  }

  if (Serial.available()>0){
    char letter = Serial.read();
    String buffer = "";
    if (int(letter)==13 || int(letter)==10 ){ //If Carriage return has been reached
      // parse string and send text to LCD
      parse_line(buffer);
    } else {
      // fill buffer
      buffer += letter;
    }
  } //If mySerial.available
}//LOOP

void parse_line( String line )
{
  // <Idle,MPos:5.529,0.560,7.000,WPos:1.529,-5.440,-0.000>
  line.replace("<", "");
  line.replace(">", "");
  // State ..
  String state = splitString(line,',',0);  

  // Machine position ...
  String machinepos_x = splitString(line,',',1).replace("MPos:", "");
  String machinepos_y = splitString(line,',',2);
  String machinepos_z = splitString(line,',',3);  

  // Working position ...
  String workingpos_x = splitString(line,',',4).replace("WPos:", "");
  String workingpos_y = splitString(line,',',5);
  String workingpos_z = splitString(line,',',6);  

  // Display on LCD ... 
  // lcd screen
  // |--------------|
  // State    X:5.529
  // Y:0.000  Z:0.000

  // XXX: to use a switch to display workpos or other things :)
  lcd.clear();
   if(option == 'm'){
      lcd.setCursor(0,0); // letter, row
      lcd.print(state);
      lcd.setCursor(9,0);
      lcd.print("X:" + machinepos_x);
      lcd.setCursor(0,1);
      lcd.print("Y:" + machinepos_y);
      lcd.setCursor(9,1);
      lcd.print("Z:" + machinepos_z);
   }
   if(option == 'w'){
      lcd.setCursor(0,0); // letter, row
      lcd.print(state);
      lcd.setCursor(9,0);
      lcd.print("X:" + workingpos_x);
      lcd.setCursor(0,1);
      lcd.print("Y:" + workingpos_y);
      lcd.setCursor(9,1);
      lcd.print("Z:" + workingpos_z);
   }
   if(option == 'd'){
      // Debug Infos
      lcd.print("Debugscreen");
   }
}

String splitString(String s, char parser,int index){
  String rs='\0';
  int parserIndex = index;
  int parserCnt=0;
  int rFromIndex=0, rToIndex=-1;

  while(index>=parserCnt){
    rFromIndex = rToIndex+1;
    rToIndex = s.indexOf(parser,rFromIndex);

    if(index == parserCnt){
      if(rToIndex == 0 || rToIndex == -1){
        return '\0';
      }
      return s.substring(rFromIndex,rToIndex);
    }
    else{
      parserCnt++;
    }

  }
  return rs;
}


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
#include <EEPROM.h>
#include <simpleThread.h>
#include <AltSoftSerial.h>
#include <Encoder.h>

/* XLCD Prototype on Arduino UNO or compatible
 * Frank (xpix) Herrmann / 07/2013
 *
 */

/* 
 * ===============================================
 * Defines
 * =============================================== 
 */

// MODE ------------------------------------------
#define MODE_PROXY   // works as proxy between PC and GRBL
//#define MODE_SPY   // works only as spy on the TX line 
                     // (no buttons, no serial console, no interaction with grbl)

// Serial speed ----------------------------------
#define PC_SERIAL       19200 // must be faster as grbl!
#define GRBL_SERIAL     9600

// LCD -------------------------------------------
//#define LCD_ADDR		   0x27  // I2C LCD Address
#define LCD_4BIT

#define LCD_cols			16
#define LCD_rows			4

#if defined(LCD_4BIT)
   #define LCD_EN          12
   #define LCD_RW          11
   #define LCD_D4          4
   #define LCD_D5          5
   #define LCD_D6          6
   #define LCD_D7          7
#elif defined(LCD_ADDR)
   #define LCD_EN          2
   #define LCD_RW          1
   #define LCD_RS          0
   #define LCD_D4          4
   #define LCD_D5          5
   #define LCD_D6          6
   #define LCD_D7          7
#endif


#if defined(MODE_PROXY)
   // Rotary Encoder --------------------------------
   #define ENC_A              2     // Encoder interrupt pin
   #define ENC_B              3     // Encoder second pin
   #define ENC_S              10    // Encoder select pin

   // Buttons ---------------------------------------
   //#define BUTTONS_A_ADC_PIN  A0    // A0 is the button ADC input A
   //#define BUTTONS_B_ADC_PIN  A1    // A1 is the button ADC input B
   //#define BUTTONS_C_ADC_PIN  A2    // A2 is the button ADC input C [optional]
   //#define BUTTONS_D_ADC_PIN  A3    // A3 is the button ADC input D [optional]
   #define BUTTONHYSTERESIS   10    // hysteresis for valid button sensing window
   #define BUTTON_NONE        0     // no pressed state
   
   // EEPROM addresses
   #define EEPROM_BUTTONS		100
   #define EEPROM_INTERVAL		150
#endif

// ========================== END Defines ========

// only for debugging
#define DEBUG

#define VERSION         0.2

// Makros
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// GRBL Serial connect pins ----------------------
#define GRBL_RX			8
#define GRBL_TX			9 

#define IDLE            1
#define QUEUE           2
#define RUN             3
#define HOLD            4
#define HOME            5
#define ALARM           6
#define CHECK           7

// Measure Power on when pressed button and note this value here
// use resistor network 680 Ohm
// Buttons:            0   1   2    3    4    5    6    7
int button_power[] = {30, 262, 415, 517, 589, 643, 685, 718}; // Power data for every pressed button

// glob Vars  ------------------------------------
const int		BUFFER_SIZE			 = 70;
byte				buttonJustPressed  = false;         //this will be true after a ReadButtons() call if triggered
byte				buttonJustReleased = false;         //this will be true after a ReadButtons() call if triggered
byte				buttonWas          = 0;             //used by ReadButtons() for detection of button events
volatile byte  button_pressed;

/* 
 * ===============================================
 * Inits
 * =============================================== 
 */
//SoftwareSerial grblSerial(GRBL_RX, GRBL_TX); // RX, TX
AltSoftSerial grblSerial;

// Add Threads to refresh status informations from GRBL
#define _sT_cnt  _sT_cnt_3    // count of threads(?, $G)
simpleThread_init(_sT_cnt);   // init threads
simpleThread_new_timebased_dynamic  (_sT_P1  , _sT_millis, 5000, _sT_start ,  getPositions);	// get position info (?)
simpleThread_new_timebased_dynamic  (_sT_P2  , _sT_millis, 5000, _sT_start ,  getStates);	// get state info ($G) (not supported from UniversalGcodeSender)
simpleThread_new_timebased_dynamic  (_sT_P3  , _sT_millis,  200, _sT_start ,  readButtons);	// get button value

// make a group
simpleThread_group_init(group_one, 2) {
   simpleThread_group(getPositions),
   simpleThread_group(getStates)
};
  

// All inits for LCD control
#if defined(LCD_4BIT)
   #include <LiquidCrystal.h>   // LCD
   LiquidCrystal myLCD(LCD_EN, LCD_RW, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
#elif defined(LCD_ADDR)
   #include <Wire.h>                // I2C Communication
   #include <LiquidCrystal_I2C.h>   // LCD over I2C
   // Set the pins on the I2C chip used for LCD connections:
   //                         addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
   LiquidCrystal_I2C myLCD(LCD_ADDR, LCD_EN, LCD_RW, LCD_RS, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 3, POSITIVE);  // Set the LCD I2C address
#endif


#ifdef ENC_A
	// RotaryEncoder
	Encoder encoder(ENC_A, ENC_B);
#endif

// LCD Menu
// look in lcd_menu.ino for build menus
// LCDMenu menu(&myLCD, LCD_cols, LCD_rows);

/* 
 * ===============================================
 * Main
 * =============================================== 
 */



/*  ---------- Setup  ----------- */

void setup() 
{ 
   Serial.begin(PC_SERIAL);   // open serial to PC

	// read or save button analog values
	get_button_values();

#ifdef ENC_S
	// set Select pin from Rotary Encoder to input
	pinMode(ENC_S, INPUT);      // sets the encoder select digital pin
#endif

   /* init threads */
	simpleThread_initSetup(_sT_cnt);
   //simpleThread_dynamic_setLoopTime(getPositions,	EEPROMReadInt(EEPROM_INTERVAL));
   //simpleThread_dynamic_setLoopTime(getStates,		EEPROMReadInt(EEPROM_INTERVAL));

   myLCD.begin(LCD_cols, LCD_rows);

   // This is the serial connect to PC, we get some commands
   // but we can also print some additional information about this module
   // and the parser from Client program will ignore this
   Serial.print(F("<XLCD "));
   Serial.print(VERSION);
   Serial.println(F(">"));
   Serial.println(F("<All commands for XLCD start with a colon ':'>"));
   Serial.println(F("<Call help with ':?'>"));

   // Old LCD Screens
   myLCD.begin(LCD_cols,LCD_rows); // letter, row

   myLCD.setCursor(0,0); // letter, row
   myLCD.print(F("XLCD "));
   myLCD.print(VERSION);
   myLCD.setCursor(0,1); // letter, row
   myLCD.print(F("Connect ... "));
   
   delay(2000);
   
   // open serial port to GRBL
   grblSerial.begin(GRBL_SERIAL); // open serial to grbl
   // reset grbl device (ctrl-X) for Universal Gcode Sender
   grblSerial.write(0x18);

   
   myLCD.clear();
}//SETUP



/*  ---------- Loop ----------- */
int pc = 0;
int gr = 0;
char pcserial[BUFFER_SIZE];
char grserial[BUFFER_SIZE];

void loop() 
{ 
   // Jobs
   simpleThread_run(_sT_priority);

   // Get data from GRBL ==> PC
   while(grblSerial.available()) {
      char c = grblSerial.read();
   
      // wait for a complete line 
      // and parse it
      if(c == '\n'){
         parseGrblLine(grserial);
   		gr = 0;
         memset(&grserial[0], 0, sizeof(grserial));
         grserial[0] = '\0';
      } else {
   		if(gr < BUFFER_SIZE)
   			grserial[gr++] = c;
      }
   
      // dont send data from $G to Serial, 
      // cuz UGS don't understand this
      // dont send data if string empty
      if(grserial[0] != '['){
         Serial.print(c);
      }
   }

  // Get data from PC ==> GRBL
   while(Serial.available()) {
      char c = Serial.read();
      
      // wait for a complete line
      // and parse it
      if(c == '\n'){
         parsePCCommand(pcserial);
         pc = 0;
         memset(&pcserial[0], 0, sizeof(pcserial));
         pcserial[0] = '\0';
      } else {
         // if to big ...
         if(pc < BUFFER_SIZE){
         	pcserial[pc++] = c;
         }
      }

      // dont send serial commands (:char) to grbl
      if(pcserial[0] != ':'){
	      grblSerial.print(c);
      }
   }
}//LOOP


// ---------- Subroutines -----------

/*
  split a string on one or more delimiter 
  and return split at index
  split(char* string[]="1,2;3")
*/
char* split( char* string, char* delimiter, int index ){
    char *ptr;

    char buffer[BUFFER_SIZE];
    strcpy(buffer, string);
    
    // init and create first cut
    ptr = strtok(buffer, delimiter);
    int x = 0;    
    while(ptr != NULL) {
      if(x++ >= index)
        break;
      // next one
      ptr = strtok(NULL, delimiter);
    }

    return ptr;
}

// Analyze every command (from PC => Xlcd) and choose an action
void parsePCCommand(char* line){
  char *c2 = strrchr(line, '\r');
  *c2 = ' ';

  // All commands with an ':' at start can control XLCD 
  if( line[0] == ':')  parse_command_line(line);
}

// Analyze every line and choose an action
void parseGrblLine(char* line){
  char *c2 = strrchr(line, '\r');
  *c2 = ' ';

  if( line[0] == '<' )  parse_status_line(line);
  if( line[0] == '[' )  parse_state_line(line);
}

// State set or get state from machine
int status = 0;

int state(char* tmp){
   if(strcmp(tmp, "Idle")==0)    status = IDLE;
   if(strcmp(tmp, "Queue")==0)   status = QUEUE;
   if(strcmp(tmp, "Run")==0)     status = RUN;
   if(strcmp(tmp, "Hold")==0)    status = HOLD;
   if(strcmp(tmp, "Home")==0)    status = HOME;
   if(strcmp(tmp, "Alarm")==0)   status = ALARM;
   if(strcmp(tmp, "Check")==0)   status = CHECK;

   return status;
}

int state(){
   return status;
}
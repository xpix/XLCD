#include <LiquidCrystal.h>   // LCD
#include <phi_interfaces.h>  // Rotary Encoder

// LCD
const int LCD_cols = 16;
const int LCD_rows = 4;

LiquidCrystal myLCD(12, 11, 4, 5, 6, 7);


#include "LCDMenu.h"  // My LCD Menu
LCDMenu menu(&myLCD, LCD_cols, LCD_rows);


// Rotary Encoder
#define ENC_A           2   // Encoder interrupt pin
#define ENC_B           A3  // Encoder second pin
#define ENC_S           3  // Encoder select pin
volatile char rotary_key;   // flag, follow button press

char mapping1[]={'R','L'}; // This is a rotary encoder so it returns U for up and D for down on the dial.
phi_rotary_encoders myEncoder(mapping1, ENC_A, ENC_B, 12);

char mapping[]={'S'}; // This is a list of names for each button.
byte pins[]={ENC_S}; // The digital pins connected to the 6 buttons.
phi_button_groups myEncoderButton(mapping, pins, 1);




void setup() 
{ 
   Serial.begin(9600);   // open serial to PC
   Serial.println("Start menu test ... ");

   myLCD.begin(LCD_cols, LCD_rows);
	
	menu_root();   
}

void loop() 
{ 
	rotary_key = myEncoder.getKey();
	if(rotary_key == NO_KEY){
			rotary_key = myEncoderButton.getKey();
	}
	switch(rotary_key){
		case 'R':
			menu.up();
			break;
		case 'L':
			menu.down();
			break;
		case 'S':
         menu.mselect();		
	}

}

void mexit()
{
	menu.mexit();
}

void menu_root(){
	menu.reinit(); // set all vars to zero

   menu.add_item("1. menuentry", menu_sub);
   menu.add_item("2. menuentry", menu_2);
   menu.add_item("3. menuentry", menu_2);
   menu.add_item("4. menuentry", menu_2);
   menu.add_item("5. menuentry", menu_2);
   menu.add_item("Exit",         mexit);
}

void menu_sub()
{
	menu.reinit(); // set all vars to zero

   menu.add_item("1. subentry", menu_2);
   menu.add_item("2. subentry", menu_2);
   menu.add_item("3. subentry", menu_2);
   menu.add_item("Back",		  menu_root);

	menu.display(0);
   
	return;
}

void menu_2(void)
{
   Serial.println("\tCallback 2");
   return;
}

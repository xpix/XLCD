/* ===============================================
 * Project: XLCD, Button Configuration
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

#ifdef BUTTONS_A_ADC_PIN

void call_button(byte number){

   if(state() != IDLE){
      Serial.println(F("<Grbl not in idle mode!>"));
      myLCD.clear();
      myLCD.print(F("<Grbl not in idle mode!>"));
      return;
   }

   #ifdef DEBUG
   Serial.print(F("<Button: "));
   Serial.print(number);
   Serial.println(F(" called>"));
   #endif

   switch (number)
   {
      case 0: call_button_0(); break;
      case 1: call_button_1(); break;
      case 2: call_button_2(); break;
      case 3: call_button_3(); break;
      case 4: call_button_4(); break;
      case 5: call_button_5(); break;
      case 6: call_button_6(); break;
      case 7: call_button_7(); break;

		// row B +10
      case 10: call_button_10(); break;
      case 11: call_button_11(); break;
      case 12: call_button_12(); break;
      case 13: call_button_13(); break;
      case 14: call_button_14(); break;
      case 15: call_button_15(); break;
      case 16: call_button_16(); break;
      case 17: call_button_17(); break;
   }
}

/* 
 * ===============================================
 * Button Configuration
 * =============================================== 
 */

void call_button_0(){ };

void call_button_1() {
   // Example call Homing
   grblSerial.print(F("$H\n"));
   
   // Display action on LCD
   myLCD.clear();
   myLCD.print(F("Homing ..."));
};

void call_button_2() {
   // example call for X+10
   grblSerial.print(F("G0 X10\n"));
};
   
void call_button_3(){ };
void call_button_4(){ };
void call_button_5(){ };
void call_button_6(){ };
void call_button_7(){ };

// Second Button row
void call_button_10(){ };
void call_button_11(){ };
void call_button_12(){ };
void call_button_13(){ };
void call_button_14(){ };
void call_button_15(){ };
void call_button_16(){ };

#endif


long encoderPosition  = -999;
/*--------------------------------------------------------------------------------------
  ReadButton()
  Detect the button pressed and return the value
  Uses global values buttonWas, buttonJustPressed, buttonJustReleased.
--------------------------------------------------------------------------------------*/
byte ReadButton()
{
   // return no button pressed if the below checks don't write to btn
   byte button = BUTTON_NONE;   

	buttonJustPressed  = false;
	buttonJustReleased = false;

#ifdef ENC_A
	// Check also RotaryEnoder Button and return this button as 'S'
	// Read Rotary direction
	long newEncoder = encoder.read();
   if(newEncoder > encoderPosition){
		button = 'R';		
	}
   if(newEncoder < encoderPosition){
	   button = 'L';
   }
	// Rotary Encoder Select
	if (digitalRead(ENC_S)) {
	   button = 'S';
	}
#endif

#ifdef BUTTONS_A_ADC_PIN
   button = checkAnalogPin(BUTTONS_A_ADC_PIN);

   //handle button flags for just pressed and just released events
   if( ( buttonWas == BUTTON_NONE ) && ( button != BUTTON_NONE ) )
   {
      //the button was just pressed, set buttonJustPressed, this can optionally be used to trigger a once-off action for a button press event
      //it's the duty of the receiver to clear these flags if it wants to detect a new button change event
      buttonJustPressed  = true;
      buttonJustReleased = false;
   }
   if( ( buttonWas != BUTTON_NONE ) && ( button == BUTTON_NONE ) )
   {
      buttonJustPressed  = false;
      buttonJustReleased = true;
   }
#endif

#ifdef DEBUG
   if(buttonWas != button && button < 40){
      Serial.print(F("<Pushed Button: "));
      Serial.print(button);
      Serial.println(F(">"));
   }
#endif

   //save the latest button value, for change event detection next time round
   buttonWas = button;   
   
   return( button );
}   

byte checkAnalogPin(byte PIN){
   byte button = -1;
   unsigned int buttonVoltage = analogRead( PIN );
   if(buttonVoltage <= 1000 + BUTTONHYSTERESIS){

#ifdef DEBUG
      Serial.print(F("<Buttonvoltage: "));
      Serial.print(buttonVoltage);
      Serial.println(F(">"));
#endif

	   // test all values to discover button
	   for (int i = COUNT_OF(button_power); i >= 0 ; i--) {
		  if( buttonVoltage < ( get_set_button_power(i,0) + BUTTONHYSTERESIS ) )
		  {
			 button = i;
		  }
	   }   
      return button;
   }
   return -1;
}
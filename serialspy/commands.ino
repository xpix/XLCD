/* ===============================================
 * Project: XLCD, user commands
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

void parse_command_line(String line)
{
   if( line.indexOf('?') == 1 ){
      // Display Menu from this device
      Serial.println(F("<XLCD 0.1 menu>"));
      Serial.println(F("<:m\tMemory free>"));

      Serial.println(F("<:b button number\tPress Button nr>"));
      Serial.println(F("<:l\tLearn button values>"));
      Serial.println(F("<:i ms\tSet or get interval in microseconds>"));

      Serial.println(F("<:r\tReset device>"));
      Serial.println(F("<:v\tAbout>"));
      return;
   }
   // Commands handle:
   if( line.indexOf('l') == 1 ) return learn_button();
   if( line.indexOf('b') == 1 ) return press_button(line);
   if( line.indexOf('i') == 1 ) return setinterval(line);
   if( line.indexOf('r') == 1 ) return resetDevice(0);
   if( line.indexOf('m') == 1 ) return memoryfree();
   if( line.indexOf('v') == 1 ) return about();
   Serial.println("Cant parse this command: " + line);
}

void learn_button(){
   Serial.println(F("learn buttons ... "));
   for (int i = 0; i <= COUNT_OF(button_power); i++)
   {
      Serial.print(F("Please push button: "));
      Serial.print(i);
      Serial.println(F(" ..."));
      unsigned int buttonVoltage = 1000;
	  boolean isB = false;
      for(;;){
         delay(200);
         buttonVoltage = analogRead( BUTTONS_A_ADC_PIN );
         if(buttonVoltage > 1000){
			buttonVoltage = analogRead( BUTTONS_B_ADC_PIN );
			if(buttonVoltage < 1000)
				isB = true;
		 }
		 if(buttonVoltage < 1000) break;
      };
	  get_set_button_power(i + (isB ? 10 : 0), buttonVoltage);
   }
   Serial.println(F("Button values saved!"));
}


void press_button(String line){
   int button = getValue(line, ' ', 1).toInt();
   if(button == 0){
      Serial.println(F("<No Button number given!>"));
      return;
   }      

   call_button(button);
   Serial.print(F("<Button: "));
   Serial.print(button);
   Serial.println(F(" pressed>"));
}


void setinterval(String line){
   int ms = getValue(line, ' ', 1).toInt();
   if(ms == 0)
      ms = simpleThread_dynamic_getLoopTime(getPositions);

   simpleThread_dynamic_setLoopTime(getPositions, ms);
   simpleThread_dynamic_setLoopTime(getStates, ms);
   Serial.print(F("<Interval: "));
   Serial.print(ms);
   Serial.println(F(">"));
}

void resetDevice(int n){
   Serial.println(F("Reset device ... "));
   delay(100);
   asm volatile ("jmp 0x0000");
}

void memoryfree(){
   Serial.print(F("<FreeMemory: "));
   Serial.print(freeMemory());
   Serial.println(F(" bytes>"));
}

void about(){
   Serial.println(F("<XLCD 0.1 / 2013>"));
   Serial.println(F("<Frank Herrmann / 2013>"));
   Serial.println(F("<https://github.com/Xpix>"));
}
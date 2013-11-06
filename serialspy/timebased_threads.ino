/* ===============================================
 * Project: XLCD, threads
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 


/* 
	************************************* 
	* Thread to get buttons
	************************************* 
	*/
void simpleThread_setup(readButtons)
{
	// output
}  

boolean simpleThread_loop(readButtons)
{

#if defined(BUTTONS_A_ADC_PIN)
	byte button = ReadButton();
	if(button >= 0 && button < 50){
		call_button(button);
	}
#endif    

	return false;    
}


/* 
	************************************* 
	* Thread to send a '?' to 
	* get GRBL position informations
	************************************* 
	*/
void simpleThread_setup(getPositions)
{
	// output
}  

boolean simpleThread_loop(getPositions)
{

   // Test when called last time and if this time less the interval time,
   if((millis() - lastCall) < simpleThread_dynamic_getLoopTime(getPositions)){
      return false;
   }

	// output counter value
	grblSerial.print('?');

#if defined(DEBUG)
	Serial.print(F("<Free "));
	Serial.print(freeRam());
	Serial.println(F(">"));
#endif
    
	return false;    
}

/* 
	************************************* 
	* Thread to send a '$G' to 
	* get GRBL position informations
	************************************* 
	*/
void simpleThread_setup(getStates)
{
	// output
}  

boolean simpleThread_loop(getStates)
{
	// output counter value
	grblSerial.print("$G\n");
    
	return false;    
}

int freeRam () {
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
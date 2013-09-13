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
    // output counter value
    grblSerial.print('?');

	#if defined(DEBUG)
		Serial.print(F("<FreeMemory: "));
		Serial.print(freeMemory());
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

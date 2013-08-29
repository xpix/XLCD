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
 * Thread to draw lcd menu
 * call menu.draw()
 ************************************* 
 */
  void simpleThread_setup(drawMenu)
  {
    // output
  }  

  boolean simpleThread_loop(drawMenu)
  {
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
    // output counter value
    grblSerial.print('?');

   Serial.print("freeMemory()=");
   Serial.println(freeMemory());

    
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

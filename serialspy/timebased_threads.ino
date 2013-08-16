/* ===============================================
 * Project: XLCD, threads
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

/* this variable is used in thread functions */
  uint8_t g_thread_counter = 0; 

/* 
 ************************************* 
 * Thread to send a '?' to 
 * get GRBL position informations
 ************************************* 
 */
  void simpleThread_setup(getPositions)
  {
    // reset counter value
    g_thread_counter = 0;
    // output
  }  

  boolean simpleThread_loop(getPositions)
  {
    // output counter value
    mySerial.print('?');
    
    // increment counter    
    g_thread_counter++;
    
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
    // reset counter value
    g_thread_counter = 0;
    // output
  }  

  boolean simpleThread_loop(getStates)
  {
    // output counter value
    mySerial.print("$G\n");
    
    // increment counter    
    g_thread_counter++;
    
    return false;    
  }

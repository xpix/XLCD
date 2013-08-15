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
  void simpleThread_setup(Loop_1)
  {
    // reset counter value
    g_thread_counter = 0;
    // output
  }  

  boolean simpleThread_loop(Loop_1)
  {
    // output counter value
    mySerial.println('?');
    
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
  void simpleThread_setup(Loop_2)
  {
    // reset counter value
    g_thread_counter = 0;
    // output
  }  

  boolean simpleThread_loop(Loop_2)
  {
    // output counter value
    mySerial.println('?');
    
    // increment counter    
    g_thread_counter++;
    
    return false;    
  }

/* ===============================================
 * Project: XLCD, Button Configuration
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

void call_button(byte number){
   switch (number)
   {
      case 1: call_button_1(); break;
      case 2: call_button_2(); break;
      case 3: call_button_3(); break;
      case 4: call_button_4(); break;
      case 5: call_button_5(); break;
      case 6: call_button_6(); break;
      case 7: call_button_7(); break;
      case 8: call_button_8(); break;
      case 9: call_button_9(); break;
      case 10: call_button_10(); break;
      case 12: call_button_12(); break;
      case 13: call_button_13(); break;
      case 14: call_button_14(); break;
      case 15: call_button_15(); break;
      case 16: call_button_16(); break;
      case 17: call_button_17(); break;
      case 18: call_button_18(); break;
      case 19: call_button_19(); break;
   }
}

/* 
 * ===============================================
 * Button Configuration
 * =============================================== 
 */

void call_button_1() {
   // Example call Homing
   grblSerial.print("$H\n");
   
   // Display action on LCD
   myLCD.clear();
   myLCD.print("Homing ...");
};

void call_button_2() {
   // example call for X+10
   grblSerial.print("G0 X10\n");
};
   
void call_button_3(){ };
void call_button_4(){ };
void call_button_5(){ };
void call_button_6(){ };
void call_button_7(){ };
void call_button_8(){ };
void call_button_9(){ };

// Second Button row
void call_button_10(){ };
void call_button_11(){ };
void call_button_12(){ };
void call_button_13(){ };
void call_button_14(){ };
void call_button_15(){ };
void call_button_16(){ };
void call_button_17(){ };
void call_button_18(){ };
void call_button_19(){ };

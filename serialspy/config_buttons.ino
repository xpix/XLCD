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

      case 'A': call_button_A(); break;
      case 'B': call_button_B(); break;
      case 'C': call_button_C(); break;
      case 'D': call_button_D(); break;
      case 'E': call_button_E(); break;
      case 'F': call_button_F(); break;
      case 'G': call_button_G(); break;
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

// Second Button row
void call_button_A(){ };
void call_button_B(){ };
void call_button_C(){ };
void call_button_D(){ };
void call_button_E(){ };
void call_button_F(){ };
void call_button_G(){ };

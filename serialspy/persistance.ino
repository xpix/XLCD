
/* ===============================================
 * Project: XLCD, persistance read and write
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

int address_buttons = 100;

int get_set_button_power(int button, int intvalue){
   // set value to eeprom
   if(intvalue > 0) 
      EEPROM.writeInt(address_buttons + button, intvalue);
   // read from eeprom
   return EEPROM.readInt(address_buttons + button);   
}
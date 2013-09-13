/* ===============================================
 * Project: XLCD, persistance read and write
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

int address_buttons = 100;

void get_button_values(){
	// Read button values from eeprom
	for(int i = 0; i <= COUNT_OF(button_power); i++){
		if(int value = get_set_button_power(i, 0)){
			if(value >= BUTTONHYSTERESIS){
				// value was saved
				button_power[i] = value;
			} else {
				// save value
				get_set_button_power(i, button_power[i]);
			}
		}
	}
}

int get_set_button_power(int button, int intvalue){
	// get button address (1 = 102, 2 = 104, ...)
   int address = (address_buttons + (2 * button));

   // set value to eeprom
	if(intvalue > 0) 
      EEPROMWriteInt(address, intvalue);

   // read from eeprom
	return EEPROMReadInt(address);
}

/* --------------- EEPROM Uthilitis ----------
	Steal from:
	http://blog.ncode.ca/?p=42
*/

//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
{
	byte lowByte = ((p_value >> 0) & 0xFF);
	byte highByte = ((p_value >> 8) & 0xFF);

	EEPROM.write(p_address, lowByte);
	EEPROM.write(p_address + 1, highByte);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
{
	byte lowByte = EEPROM.read(p_address);
	byte highByte = EEPROM.read(p_address + 1);

	return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

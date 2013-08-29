/* ===============================================
 * Project: XLCD, LCD MENWIZ Menu
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

/*
.___  ___.  _______ .__   __.  __    __
|   \/   | |   ____||  \ |  | |  |  |  |
|  \  /  | |  |__   |   \|  | |  |  |  |
|  |\/|  | |   __|  |  . `  | |  |  |  |
|  |  |  | |  |____ |  |\   | |  `--'  |
|__|  |__| |_______||__| \__|  \______/
*/


// Display menu at offset
void menu_display(int offset) {

	// roll over to first entry
	if(LCD_offset >= menucounter)
	LCD_offset = 0;
	if(LCD_offset < 0)
	LCD_offset = menucounter;
	if(LCD_offset >= (menucounter - LCD_rows))
	LCD_offset = (menucounter - LCD_rows);

	if(LCD_select >= menucounter){
		LCD_offset = 0;
		LCD_select = 0;
	}
	if(LCD_select < 0){
		LCD_select = menucounter - 1;
	}
	
	struct menuitem *item;

	myLCD.clear();

	// Display menu structure
	int pos = 0;
	for (int i = LCD_offset; i <= menucounter; i++)
	{
		if(pos >= LCD_rows){
			break;
		}
		// Display a '-' for selected point
		myLCD.setCursor(0, pos);
		myLCD.print((i == LCD_select ? LCD_select_char : ' '));

		// Display menu text
		item = &items[i];
		myLCD.setCursor(1, pos);
		myLCD.print(item->name);
		
		pos++;
	}
};

// Call menu point
void menu_select(int select) {

	struct menuitem *item;
	item = &items[select];
	if(item->callback)
		item->callback();
}

void menu_about(void)
{
	LCD_offset = -1;
	myLCD.clear();
	myLCD.print(F("XPIX Development\n08 / 2013 V 0.1"));

	return;
}

void menu_exit(void)
{
	LCD_offset = -1;
	Serial.println("\tExit ..");
	myLCD.clear();
	
	return;
}

void menu_1(void)
{
	Serial.println("\tCallback 1");
	return;
}

void menu_2(void)
{
	Serial.println("\tCallback 2");
	return;
}

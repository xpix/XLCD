/* 
* LCDMenu.h
*
* Created: 27.08.2013 15:21:24
* Author: C5125630
*
* Please use only this LCD lib:
* https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
*/


#ifndef LCDMenu_h
#define LCDMenu_h

#include <LCD.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define MY_LCD         LCD   // this could help to change the library: your lcd data type

class LCDMenu
{
	//functions
	public:
		LCDMenu(void *,int, int);
		void add_item(const char*, void (*)(int), int);
		void add_item(const __FlashStringHelper *, void (*)(int), int);
		void display(int);
		void mselect();
		void about(String);
		void mexit();
		void up();
		void down();
		void reinit();
		void onchange(void (*)(int));				

		MY_LCD*  lcd;

	private:
		char buffer[21];

		struct menuitem
		{
			char name[20];
			void (*callback)(int);
			int callback_param;
		};

		// maximum entrys (10)
		struct menuitem items[10];


}; //LCDMenu

#endif //__LCDMENU_H__

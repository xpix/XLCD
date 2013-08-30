/* ===============================================
 * Project: XLCD, LCD MENWIZ Menu
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: [8/30/2013 c5125630]
 */ 

void menu_onchange(int state){
	if(state == 0){
		// Restart Threads if call exit Menu
		simpleThread_restart(getPositions);
		simpleThread_restart(getStates);
	}
	else {
		// Stop Threads if menu displayed
		simpleThread_stop(getPositions);
		simpleThread_stop(getStates);
	}
}

void menu_root(int disp){
	menu.reinit(); // set all vars to zero

	// Register onchange event callback
	menu.onchange(menu_onchange);

	// LCD size:  |----------------|---|
	menu.add_item("Set serial mode",   menu_mode,      0);
	menu.add_item("Set data interval", menu_interval,  0);
	menu.add_item("Learn buttons",     menu_learn,     0);
	menu.add_item("Reset XLCD",        resetDevice,    0);
	menu.add_item("About",			     menu_about,     0);
	menu.add_item("Exit",              menu_exit,      0);

	if(disp)
		menu.display(0);
}

void menu_mode(int n)
{
	menu.reinit(); // set all vars to zero

	// LCD size:  |----------------|---|
	menu.add_item(strcat("Spy mode",		(proxymode ? "" : "on")),	menu_mode_set, 0);
	menu.add_item(strcat("Proxy mode",	(proxymode ? "on" : "")),  menu_mode_set, 1);
	menu.add_item("Back",               menu_root, 1);

	menu.display(0);
}

void menu_mode_set(int on)
{
   if(on == 1){
	   proxymode = true;
   } else {
	   proxymode = false;
   }
	myLCD.clear();
	// LCD size:  |----------------|---|
	myLCD.print(strcat("Set mode: ",( proxymode ? "proxy" : "spy" )));

	delay(1000);
	menu_mode(0);
}


void menu_about(int n)
{
	myLCD.clear();
	// LCD size:  |----------------|---|
	myLCD.println("XLCD 0.1/08.2013");
	myLCD.println("Frank Herrmann");
	delay(1000);
	menu_root(1);
}

void menu_interval(int n)
{
	menu.reinit(); // set all vars to zero

	int ms = simpleThread_dynamic_getLoopTime(getPositions);

	// LCD size:  |----------------|---|
	menu.add_item(strcat("  0.2s",(ms == 200 ? "*" : "")), _setInterval, 200);
	menu.add_item(strcat("  0.5s",(ms == 500 ? "*" : "")), _setInterval, 500);
	menu.add_item(strcat("  1.0s",(ms == 1000 ? "*" : "")), _setInterval, 1000);
	menu.add_item(strcat("  2.0s",(ms == 2000 ? "*" : "")), _setInterval, 2000);
	menu.add_item(strcat("  5.0s",(ms == 5000 ? "*" : "")), _setInterval, 5000);
	menu.add_item(strcat(" 30.0s",(ms == 30000 ? "*" : "")), _setInterval, 30000);

	menu.add_item("Back",		menu_root,		1);

	menu.display(0);
}

void _setInterval(int ms){
	myLCD.clear();
	String setstr = ":i " + ms;
	setinterval(setstr);
	menu_interval(0);
}

void menu_learn(int n)
{
}

void menu_exit(int n)
{
	menu.mexit();
}


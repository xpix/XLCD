/* ===============================================
 * Project: XLCD, LCD Screens
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

unsigned long last_status = 0;

void parse_status_line(char* line)
{
   // <Idle,MPos:5.529,0.560,7.000,WPos:1.529,-5.440,-0.000>

   // Display on LCD ...
   // lcd screen
   // |--------------|
   // State  Z:555.529
   // 000.000  000.000

   // XXX: to use a switch to display work position or other things :)
   myLCD.clear();

	char delim[] = "<,:";

   // State ..
   char* temp = split(line, delim, 0);
   myLCD.setCursor(0,0); // letter, row
   myLCD.print(temp);

   // X Machine position ...
   temp = split(line, delim, 2);
   myLCD.setCursor(0,1);
   myLCD.print(temp);

   // Y Machine position ...
   temp = split(line, delim, 3);
   size_t len = strlen(temp);
	myLCD.setCursor((LCD_cols - len),1);
   myLCD.print(temp);

   // Z Machine position ...
   temp = split(line, delim, 4);
   len = strlen(temp);
   myLCD.setCursor((LCD_cols - len),0);
   myLCD.print(temp);
}

// send every second the command $G
void parse_state_line(char* myBuffer)
{
   // Display on LCD ...
   // |--------------|
   // S1 T1 F1000
   // MM LIN XY M1

	char delim[] = "[ ]";

   //             mm                   TNr Feed
   // G0 G54 G17 G21 G90 G94 M0 M5 M9 T0 F500.000

   // Feed
   char *temp = split(split(myBuffer, delim, 10), ".", 0);
   myLCD.setCursor(6,2); //third row
   myLCD.print(temp);

   // Tool
   temp = split(myBuffer, delim, 9);
   myLCD.setCursor(3,2);
   myLCD.print(temp);

   temp = split(myBuffer, delim, 7);
   if(temp == "M5") temp = "on ";
   if(temp != "M5") temp = "off";
   myLCD.setCursor(0,2); //third row
   myLCD.print(temp);

   // last row
   // Move
   temp = split(myBuffer, delim, 0);
   if(temp == "G")  temp = "RAP";
   if(temp == "G0") temp = "RAP";
   if(temp == "G1") temp = "LIN";
   if(temp == "G2") temp = "CW ";
   if(temp == "G3") temp = "CCW";
   myLCD.setCursor(3,2); //third row
   myLCD.print(temp);

   // mm or inch
   temp = split(myBuffer, delim, 3);
   if(temp == "G21"){ 
     temp = "MM";
   } else {
     temp = "IN";
   }
   myLCD.setCursor(0,3);
   myLCD.print(temp);

   // Plane
   temp = split(myBuffer, delim, 2);
   if(temp == "G17") temp = "XY";
   if(temp == "G18") temp = "ZX";
   if(temp == "G19") temp = "YZ";
   myLCD.setCursor(7,3);
   myLCD.print(temp);

   // Flow
   temp = split(myBuffer, delim, 6);
   myLCD.setCursor(10,3);
   myLCD.print(temp);
}



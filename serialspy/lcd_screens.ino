/* ===============================================
 * Project: XLCD, LCD Screens
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

unsigned long lastCall = 0;

void parse_status_line(char* line)
{
   // <Idle,MPos:5.529,0.560,7.000,WPos:1.529,-5.440,-0.000>

   // Display on LCD ...
   // lcd screen
   // |--------------|
   // State  Z:555.529
   // 000.000  000.000

   // XXX: to use a switch to display work position or other things :)
   //myLCD.clear();

	char delim[] = "<,:";

   // State ..
   myLCD.setCursor(0,0); // letter, row
   myLCD.print("     ");
   char* temp = split(line, delim, 0);
   myLCD.setCursor(0,0); // letter, row
   myLCD.print(temp);
   state(temp);
   
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

   lastCall = millis();
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

   char *temp = split(myBuffer, delim, 7);
   if( strcmp(temp, "M5")==0) 
      temp = "On ";
   if( strcmp(temp, "M5")!=0) 
      temp = "Off";
   myLCD.setCursor(0,2); //third row
   myLCD.print(temp);

   myLCD.print(" ");

   // Move
   temp = split(myBuffer, delim, 0);
   if(strcmp(temp, "G")==0 || strcmp(temp, "G0")==0)  
      myLCD.print("RAP");
   if(strcmp(temp, "G1")==0)
      myLCD.print("LIN");
   if(strcmp(temp, "G2")==0)
      myLCD.print("CW ");
   if(strcmp(temp, "G3")==0)
      myLCD.print("CCW");

   // Feed
   temp = split(split(myBuffer, delim, 10), ".", 0);
   char *c2 = strrchr(temp, 'F');
   *c2 = ' ';
   size_t len = strlen(temp) + 4;
   myLCD.setCursor((LCD_cols - len),2);
   myLCD.print(strcat(temp,"mm/s"));

   // next line
   myLCD.setCursor(0,3);

   // mm or inch
   temp = split(myBuffer, delim, 3);
   if(strcmp(temp, "G21")==0){ 
      myLCD.print("mm");
   } else {
      myLCD.print("in");
   }

   myLCD.print(" ");

   // Plane
   temp = split(myBuffer, delim, 2);
   if(strcmp(temp, "G17")==0) temp = "XY";
   if(strcmp(temp, "G18")==0) temp = "ZX";
   if(strcmp(temp, "G19")==0) temp = "YZ";
   myLCD.print(temp);

   myLCD.print(" ");

   // Tool
   temp = split(myBuffer, delim, 9);
   myLCD.print(temp);

   myLCD.print(" ");

   // Flow (Pause, Stop ...)
   temp = split(myBuffer, delim, 6);
   if(strcmp(temp, "M0")==0) temp = "Pause";
   if(strcmp(temp, "M2")==0) temp = "End";
   if(strcmp(temp, "M30")==0) temp = "End";
   len = strlen(temp);
   myLCD.setCursor((LCD_cols - len),3);
   myLCD.print(temp);


}



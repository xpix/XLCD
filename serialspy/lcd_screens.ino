/* ===============================================
 * Project: XLCD, LCD Screens
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

unsigned long last_status = 0;

void parse_status_line(String line)
{
   // <Idle,MPos:5.529,0.560,7.000,WPos:1.529,-5.440,-0.000>

   // analyze last call time and ignore this event if last time smaller as interval
   if(millis() - last_status < simpleThread_dynamic_getLoopTime(getPositions)) 
      return;

   last_status = millis();

   // State ..
   String state = getValue(getValue(line, ',', 0), '<', 1);

   // Machine position ...
   String machinepos_x = getValue(getValue(line, ',', 1), ':', 1);
   String machinepos_y = getValue(line, ',', 2);
   String machinepos_z = getValue(line, ',', 3);

   // Working position ...
   String workingpos_x = getValue(getValue(line, ',', 4), ':', 1);
   String workingpos_y = getValue(line, ',', 5);
   String workingpos_z = getValue(getValue(line, ',', 6), '>', 0);

   // return if not state
   if(workingpos_z.length() == 0){
      return;
   }

   // Display on LCD ...
   // lcd screen
   // |--------------|
   // State  Z:555.529
   // 000.000  000.000

   // XXX: to use a switch to display work position or other things :)
   // myLCD.clear();
   // Display State and Machine position in first 2 rows
   myLCD.setCursor(0,0); // letter, row
   myLCD.print(state + "            ");
   myLCD.setCursor((LCD_cols-machinepos_z.length()),0);
   myLCD.print(machinepos_z);

   myLCD.setCursor(0,1);
   myLCD.print(machinepos_x);
   myLCD.setCursor((LCD_cols-machinepos_y.length()),1);
   myLCD.print(machinepos_y);
}

// send every second the command $G
void parse_state_line(String myBuffer)
{
   // parse myBuffer
   myBuffer.replace("[","");
   myBuffer.replace("]","");
   //             mm                   TNr Feed
   // [G0 G54 G17 G21 G90 G94 M0 M5 M9 T0 F500.000]
   String move = getValue(myBuffer, ' ', 0);
   if(move == "G")  move = "RAP";
   if(move == "G0") move = "RAP";
   if(move == "G1") move = "LIN";
   if(move == "G2") move = "CW ";
   if(move == "G3") move = "CCW";

   String plane = getValue(myBuffer, ' ', 2);
   if(plane == "G17") plane = "XY";
   if(plane == "G18") plane = "ZX";
   if(plane == "G19") plane = "YZ";

   String feedrate = getValue(myBuffer, ' ', 10);
   feedrate.replace(".000","");

   String toolnr = getValue(myBuffer, ' ', 9);
   String spindle = (getValue(myBuffer, ' ', 7) == "M5" ? "1" : "0");

   String mode = (getValue(myBuffer, ' ', 3) == "G21" ? "MM" : "IN");
   String flow = getValue(myBuffer, ' ', 6);

   // Display on LCD ...
   // |--------------|
   // S1 T1 F1000
   // MM LIN XY M1

   // Spindle
   myLCD.setCursor(0,2); //third row
   myLCD.print("S" + spindle);

   // Tool
   myLCD.setCursor(3,2);
   myLCD.print(toolnr);

   // Feed
   myLCD.setCursor(6,2); //third row
   myLCD.print(feedrate);


   // last row

   // mm or inch
   myLCD.setCursor(0,3);
   myLCD.print(mode);

   // Move mode
   myLCD.setCursor(3,3);
   myLCD.print(move);

   // Plane
   myLCD.setCursor(7,3);
   myLCD.print(plane);

   // Flow
   myLCD.setCursor(10,3);
   myLCD.print(flow);

}



/* ===============================================
 * Project: XLCD, user commands
 * ===============================================
 * Autor:     Frank (xpix) Herrmann
 * Email:     xpixer@gmail.com
 * License:   all Free
 * Last edit: 30.08.2013
 */ 

void parse_command_line(String line)
{
   if( line.indexOf('?') == 1 ){
      // Display Menu from this device
      Serial.println(F("<XLCD 0.1 menu>"));
      Serial.println(F("<:m\tMemory free>"));
      Serial.println(F("<:i ms\tSet or get interval in microseconds>"));

      Serial.println(F("<:r\tReset device>"));
      Serial.println(F("<:v\tAbout>"));
      return;
   }
   // Commands handle:
   if( line.indexOf('i') == 1 ) return setinterval(line);
   if( line.indexOf('r') == 1 ) return resetDevice();
   if( line.indexOf('m') == 1 ) return memoryfree();
   if( line.indexOf('v') == 1 ) return about();
   Serial.println("Cant parse this command: " + line);
}

void setinterval(String line){
   int ms = getValue(line, ' ', 1).toInt();
   if(ms == 0)
      ms = simpleThread_dynamic_getLoopTime(getPositions);

   simpleThread_dynamic_setLoopTime(getPositions, ms);
   simpleThread_dynamic_setLoopTime(getStates, ms);
   Serial.print(F("<Interval: "));
   Serial.print(ms);
   Serial.println(F(">"));
}

void resetDevice(){
   Serial.println(F("Reset device ... "));
   delay(100);
   asm volatile ("jmp 0x0000");
}

void memoryfree(){
   Serial.print(F("<FreeMemory: "));
   Serial.print(freeMemory());
   Serial.println(F(" bytes>"));
}

void about(){
   Serial.println(F("<XLCD 0.1 / 2013>"));
   Serial.println(F("<Frank Herrmann / 2013>"));
   Serial.println(F("<https://github.com/Xpix>"));
}

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
      Serial.println("XLCD 0.1 menu:");
      Serial.println(":r\tReset device");

      Serial.println(":v\tAbout");
      return;
   }
   if( line.indexOf('r') == 1 ) return resetDevice();
   if( line.indexOf('v') == 1 ) return about();
   Serial.println("Cant parse this command: " + line);
}

void resetDevice(){
   Serial.println("Reset device ... ");
   delay(100);
   asm volatile ("jmp 0x0000");
}

void about(){
   Serial.println("About XLCD 0.1");
   Serial.println("Frank Herrmann / 2013");
   Serial.println("https://github.com/Xpix");
}

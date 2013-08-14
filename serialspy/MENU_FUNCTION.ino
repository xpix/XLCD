/* ==========================
 * Init Screen
 *
 * Beschreibung: 
 * Diese Funktion zeigt einen Text an und wird beendet,
 * sobald ein Button betätigt wird
 * ========================= */ 
  void FUNC_init_screen(void)
  {
  /* --------- INIT ---------
     * Initialisierung dieser Funktion
     * wird nur einmal beim Start dieser Funktion ausgefuert
     * danach nur erneut, wenn die Menu Funktion verlassen wurde
     * wenn keine Variablen initialisiert werden müssen, kann diese 
     * Funktion anstatt mit if(!LCDML.FuncInit()) ....     mit 
     * LCDML.FuncInit();  aufgerufen werden     
     */      
    if(!LCDML.FuncInit())
    { 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("XLCD 0.1");      
      lcd.setCursor(0,1);
      lcd.print("Frank Herrmann / 2013");
    }  
    /* --------- LOOP ----------
     * Hier sollte der Code stehen der ständig neu geladen wird 
     * z.B. eine Uhrzeit oder änliches.
     */  
    //Serial.println("init_screen");
    
    
    /* --------- STOP  ---------      
     * LCDML.FuncEnd (direct, enter, up, down, left, right)
     * Fall in dieser Funktion keine Variablen zurückgesetzt werden 
     * müssen, kann diese Funktion auch vereinfacht mit CTR_end(0, 1, 1, 1, 1, 1);
     * aufgerufen werden. Die Endbedingungen müssen durch die Zahlen >0 vorgegeben 
     * werden
     * Wenn alles 0 ist, hängt das gesammte Programm in dieser Funktion fest
     */
    if(LCDML.FuncEnd(0, 1, 1, 1, 1, 1)) 
    {
      /* Falls noch Variablen zurückgesetzt werden müssen, bevor diese Funktion
       * verlassen wird, kann dies in dieser If Schleife geschehen
       */       
    }
  }

/* ==========================
 * Information 
 *
 * Beschreibung: 
 * Diese Funktion zeigt einen Text an und wird beendet,
 * sobald ein Button betätigt wird
 * ========================= */ 
  void FUNC_information(void)
  {       
    /* --------- INIT ---------
     * Initialisierung dieser Funktion
     * wird nur einmal beim Start dieser Funktion ausgefuert
     * danach nur erneut, wenn die Menu Funktion verlassen wurde
     * wenn keine Variablen initialisiert werden müssen, kann diese 
     * Funktion anstatt mit if(!LCDML.FuncInit()) ....     mit 
     * LCDML.FuncInit();  aufgerufen werden     
     */   
    if(!LCDML.FuncInit()) 
    {
      /* Init Function */             
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("Um Funktion zu"));
      lcd.setCursor(0,1);
      lcd.print(F("schliessen eine"));
      lcd.setCursor(0,2);
      lcd.print(F("Taste druecken oder"));
      lcd.setCursor(0,3);
      lcd.print(F("Back Taste verwenden"));      
    } 
    /* --------- LOOP ----------
     * Hier sollte der Code stehen der ständig neu geladen wird 
     * z.B. eine Uhrzeit oder änliches.
     */    
    
    /* --------- STOP  ---------      
     * LCDML.FuncEnd (direct, enter, up, down, left, right)
     * Fall in dieser Funktion keine Variablen zurückgesetzt werden 
     * müssen, kann diese Funktion auch vereinfacht mit CTR_end(0, 1, 1, 1, 1, 1);
     * aufgerufen werden. Die Endbedingungen müssen durch die Zahlen >0 vorgegeben 
     * werden
     * Wenn alles 0 ist, hängt das gesammte Programm in dieser Funktion fest
     */
    if(LCDML.FuncEnd(0, 1, 1, 1, 1, 1)) 
    {
      /* Falls noch Variablen zurückgesetzt werden müssen, bevor diese Funktion
       * verlassen wird, kann dies in dieser If Schleife geschehen
       */
    }     
  }




/* ==========================
 * Timer Info 
 *
 * Beschreibung: 
 * Diese Funktion zählt einen Wert von 9 im eine Sekunden Takt 
 * hinunter, sobald der Wert den Wert 0 erreicht, wird die Funktion
 * geschlossen
 * ========================= */ 
/* Globale Variablen für diese Funktion */ 
  uint8_t g_func_timer_info = 0;  /* Merker der von 10 herab gezählt wird */
  unsigned long g_timer_1 = 0;    /* Variable für den Timer der jede Sekunde ausgelöst wird */
  
  void FUNC_timer_info(void)
  {
    /* --------- INIT ---------
     * Initialisierung dieser Funktion
     * wird nur einmal beim Start dieser Funktion ausgefuert
     * danach nur erneut, wenn die Menu Funktion verlassen wurde
     * wenn keine Variablen initialisiert werden müssen, kann diese 
     * Funktion anstatt mit if(!LCDML.FuncInit()) ....     mit 
     * LCDML.FuncInit();  aufgerufen werden     
     */  
    if(!LCDML.FuncInit()) 
    {        
      /* Hier wird das Display einmalig gelöscht und Text 
       * ausgegeben, der sich nicht ändert
       * Außerdem wird hier eine Variable mit dem Wert 10
       * initialisiert
       */      
      lcd.clear();
      lcd.print(F("x sec warten"));      
      g_func_timer_info = 10;      
    }
    
    /* --------- LOOP ----------
     * Mit der Timer Funktion wird festgelegt in welchen Abständen ein 
     * Ereignis erlaubt ist     
     */
    /* LCDML.Timer(Timer Variable im unsigned long Format, Zeit die gewartet wird in ms) */    
    if(LCDML.Timer(g_timer_1, 1000)) {
      /* Wird nur jede Sekunde ausgeführt */
      g_func_timer_info--;
      lcd.setCursor(0,0);
      lcd.print(g_func_timer_info);     
    }     
    
    /* --------- STOP  ---------      
     * LCDML.FuncEnd (direct, enter, up, down, left, right)
     * Fall in dieser Funktion keine Variablen zurückgesetzt werden 
     * müssen, kann diese Funktion auch vereinfacht mit CTR_end(0, 1, 1, 1, 1, 1);
     * aufgerufen werden. Die Endbedingungen müssen durch die Zahlen >0 vorgegeben 
     * werden
     * Wenn alles 0 ist, hängt das gesammte Programm in dieser Funktion fest
     *
     * In diesem Fall wird die Stop Funktion nur abgefragt, wenn die Bedingung
     * davor erfüllt ist
     */
    if(g_func_timer_info <= 0) 
    {
      if(LCDML.FuncEnd(1, 0, 0, 0, 0, 0)) 
      {
         /* Falls noch Variablen zurückgesetzt werden müssen, bevor diese Funktion
          * verlassen wird, kann dies in dieser If Schleife geschehen
          */        
      }
    }    
  } 



/* ==========================
 * Programm 2 (p2) 
 *
 * Beschreibung: 
 * Diese Funktion wird erst geschlossen, wenn 3 mal nach links gedrückt wurde
 * ========================= */ 
  void FUNC_p2(void)
  {    
    /* --------- INIT ---------
     * Initialisierung dieser Funktion
     * wird nur einmal beim Start dieser Funktion ausgefuert
     * danach nur erneut, wenn die Menu Funktion verlassen wurde
     * wenn keine Variablen initialisiert werden müssen, kann diese 
     * Funktion anstatt mit if(!LCDML.FuncInit()) ....     mit 
     * LCDML.FuncInit();  aufgerufen werden     
     */  
    if(!LCDML.FuncInit()) {
      /* Init Function */             
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("3 mal Links drucken"));
      lcd.setCursor(0,1);
      lcd.print(F("Anzahl: 0"));      
    }
    
    /* --------- LOOP ----------
     * Hier wird nur ein Ereignis ausgelöst, sobald ein Button gedrückt
     * wurde. Die Funktion LCDML.checkButtons liefert true, wenn ein Button 
     * seit dem letztem durchlauf betätigt wurde, ansonsten false.     
     */
    if(LCDML.checkButtons()) {
      /* Die Position des Cursor wird verschoben */
      lcd.setCursor(8,1);
      /* Die Anzahl wie oft ein Button gedrückt wurde, wird angezeigt */
      lcd.print(LCDML.g_button_left);
    }    
    
    /* --------- STOP  ---------      
     * LCDML.FuncEnd (direct, enter, up, down, left, right)
     * Fall in dieser Funktion keine Variablen zurückgesetzt werden 
     * müssen, kann diese Funktion auch vereinfacht mit CTR_end(0, 1, 1, 1, 1, 1);
     * aufgerufen werden. Die Endbedingungen müssen durch die Zahlen >0 vorgegeben 
     * werden
     * Wenn alles 0 ist, hängt das gesammte Programm in dieser Funktion fest
     */
    if(LCDML.FuncEnd(0, 0, 0, 0, 3, 0)) 
    {
      /* Falls noch Variablen zurückgesetzt werden müssen, bevor diese Funktion
       * verlassen wird, kann dies in dieser If Schleife geschehen
       */
    }      
  }
  
  

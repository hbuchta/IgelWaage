#include "header.h"

// Tipp aus https://www.youtube.com/watch?v=--KxxMaiwSE&list=PL2GFrrKBMENgmugJ3yJIuGFLavfqgGPFe&index=82
#define DEBUG 0

#if DEBUG==1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif


int operation_mode = OPERATION_MODE.RUN;



// Speziell ESP 07S
// https://smartsolutions4home.com/how-to-program-esp8266/

void setup() {  
  #if DEBUG==1
  Serial.begin(115200);
  #endif
  
  pinMode(CONFIG_BUTTON, INPUT);
  EEPROM.begin(1024);
  yield();

  bool config_ok = load_config(configdata);
  checkconfig(configdata, webconfig);   
  
  debug("\n\n\nWiFi Scale Version ");  
  debug(release_version);
  debug(", Konfiguration Version ");
  debugln(configdata.release_version);
  #if DEBUG==1
    Serial.printf(
    "\n\nSketchname: %s\nBuild: %s\t\tIDE: %d.%d.%d\n%s\n\n"
    ,(__FILE__), (__TIMESTAMP__), ARDUINO / 10000, ARDUINO % 10000 / 100, ARDUINO % 100 / 10 ? ARDUINO % 100 : ARDUINO % 10, ESP.getFullVersion().c_str()
    );
  #endif
  debug("MAC: ");
  debugln(WiFi.macAddress());
  debug("Chip-ID: 0x");
  debugln(String(ESP.getChipId(), HEX));
  debug("Speichergröße Konfigurationsdaten: ");
  debug(sizeof(eeromstruct));  
  debugln(" Byte");
  debugln(webconfig.all_ok?"Konfiguration vollständig":"Konfiguration unvollständig");    
  

  if (digitalRead(CONFIG_BUTTON)==HIGH | !webconfig.all_ok) {
    // Konfigurationsmodus der Waage
    
    debugln("\nKonfigurationsmodus");
    
    
    operation_mode = OPERATION_MODE.CONFIG;
    
    start_config_server();
  } else {
    
    // Wiegemodus / Normalbetrieb
    operation_mode = OPERATION_MODE.RUN;

    // Konfiguration und letzten Vorgang laden
    rtc = readrtc();    
	  rtc.starttime = rtc.timestamp+rtc.sleeptime;	// Letzte Schlafdauer zu Startzeit dazu addieren 	
    // Daten der letzten Wiegung lesen (sofern vorhanden)     
        
    debug("\nWiFi Scale Version ");
    debugln(configdata.release_version);
    debugln(rtc.lastweightok?"Neuerstart der Waage":"Waage aus Deepsleep aufgewacht");
    
    
    // WiFi noch ausgeschaltet lassen
    WiFi.forceSleepBegin();
    delay(1);     
  }

  // Waage starten (wir lassen sie an, da im Normalmodus direkt im Anschluss die Wiegung erfolgt
  ESP.wdtDisable();  // Watchdog
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);    
  ESP.wdtEnable(2000); 
  delay(10);    
}

void loop() {   
  if (operation_mode == OPERATION_MODE.CONFIG) {
    // Webserver fuer Konfiguration
    dnsServer.processNextRequest();
    server.handleClient();
  } else {    
     // Wiegung durchfuehren
    process_scale();

    if (supply_power<3.0) {
      // Batterie praktisch leer. Waage wird abgeschaltet.
      scale.power_down();
      
      debugln();
      
      debug("Batteriespannung niedrig: ");
      debug(supply_power);
      debugln("V");
      debugln("Deep Sleep wird ausgelöst, manueller Eingriff erforderlich.");
              
      ESP.deepSleep(0);   // Deep sleep ohne automatisches Aufwachen
      delay(10);  
      return;
    } else {            
      
      debug("Aktuelle Betriebsspannung: ");
      debug(supply_power); 
      debugln("V");
      debugln("Deep Sleep wird ausgelöst");  
      
            
      delay(1);
	  
  	  // Schlafzeit: Von der konfigurierten Zeit wird die aktuelle Laufzeit abgezogen
  	  rtc.sleeptime = configdata.sleeptime - millis()/1000;
  	  
  	  if (rtc.sleeptime<5) rtc.sleeptime=5; // Minimum: 5 Sekunden

      
      debug("Berechnete Schlafzeit: ");
      debug(rtc.sleeptime);
      debugln(" Sekunden");
      
  	  
      writertc(rtc); // Aktuelle Uhrzeit speichern
      yield();
      // Aufwachen durch Reset nach Timeout (Konfiguration, sofern Verbindung D0-RST vorhanden ist)           
      ESP.deepSleep(rtc.sleeptime*1e6, WAKE_RF_DEFAULT);  
      delay(10);   
    }   
  }  
}  

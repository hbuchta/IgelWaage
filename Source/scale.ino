
////////////////////////////////////////////
// Waage auslesen
// Es werden count Messungen in Folge gemacht, deren Min/Max Werte nicht mehr
// als minmax_threshold auseinanderliegen dürfen (ansonsten fangen die count
// Messungen wieder von vorne an.
// Werden insg. mehr als max_iterations Messungen benötigt, wird das letzte Ergebnis
// geliefert. Dauert es länger als timetolife_threshold Millisekunden, bevor die Waage antwortet,
// wird davon ausgegangen, dass keine Waage angeschlossen ist.
// Am Ende werden Min und Max abgezogen und von den verbleibenden Werten wird der 
// Durchschnitt berechnet.
//
// Ergebnis in Struktur hx711result
// weight      Ergebnis der Wiegung (noch nicht skaliert)
// iterations  Anzahl Einzelmessungen, bis eine Gesamtmessung entstanden ist        
// status      0:ok, 1:keine stabile Messung, 2: timeout an der Waage/keine Waage angeschlossen?
struct hx711result getweight(HX711 scale, int count, long minmax_threshold, int max_iterations, long timetolife_threshold) {
  long weight_min=INT32_MAX ;
  long weight_max=INT32_MIN;
  long weightsum=0;
  int iterations=0;
  int valuecount=0;
  bool status=0;

  if (count<1) count=1;

  while (true) {     
    status=0;
    iterations++;    
    if (iterations>=max_iterations) {
      status=1; // Überschreitung der Messversuche: keine stabile Messung möglich!
      break;
    }   
    long start_millis = millis();
    while (!scale.is_ready()) {
      if ((millis()-start_millis)>timetolife_threshold) {
        status=2;
        break;
      }
      delay(100);     
    }
    if (status==2) break;
    long weight =  scale.read();    
    weightsum += weight;   
    valuecount++; 
    
    if (weight<weight_min) weight_min = weight;
    if (weight>weight_max) weight_max = weight;
    
    if (weight_max-weight_min>minmax_threshold) {     
      // nochmal von vorne...
      weightsum=0;
      valuecount=0;
      weight_min=INT32_MAX;
      weight_max=INT32_MIN;
    } else if (valuecount>=count) {
      // Ergebnis der Messung liegt vor
      break;      
    }
  }
  // größten/kleinsten Wert entfernen und Durchschnitt der übrigen Werte nehmen  
  float weightavg;
  if (valuecount>=10) {
    weightavg = (float)(weightsum-weight_min-weight_max) / (valuecount-2); 
  } else if (valuecount>2) {
    weightavg = (float)(weightsum) / valuecount; 
  } else {
    weightavg = (float)weightsum;
  }
  
  hx711result ret;
  ret.weight = weightavg;
  ret.status = status;
  ret.iterations = iterations;
  ret.weight_min = weight_min;
  ret.weight_max = weight_max;
  return(ret);
}

struct hx711result getweight(HX711 scale) {
  return(getweight(scale, 15, 350, 100, 5000));
}

void startWifi() {
  
  debug("Connecting to ");
  debugln(configdata.wifi_ssid);  
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
    
  WiFi.begin(configdata.wifi_ssid, configdata.wifi_pwd);
  while (WiFi.status() != WL_CONNECTED) {       
    
    debug(".");
    
    delay(500);
  } 
    
  debug("\nWiFi connected, IP address: ");  
  debugln(WiFi.localIP());
  
  
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  // Set time via NTP, as required for x.509 validation
  // https://praxistipps.chip.de/fritzbox-als-ntp-server-verwenden-so-gehts_93921
  //configTime(3 * 3600, 0, "fritz.box", "pool.ntp.org", "time.nist.gov");
  configTime(3 * 3600, 0, configdata.ntp_server);
  
  
  debug("Waiting for NTP time sync");
  
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    
    debug(".");
    
    now = time(nullptr);
  }  
  debugln();
  struct tm timeinfo;
  
  setenv("TZ", "TZ=CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();      
}


rtcmem readrtc() {
  rtcmem rtc;
  ESP.rtcUserMemoryRead(0, (uint32_t*)&rtc, sizeof(rtc));

  if (String(rtc.check)=="WFW2") {
    
    debug("Letzte Wiegung gefunden, Timestamp: ");
    debug(rtc.timestamp);
    debug(", Gewicht: ");
    debugln(rtc.lastweight);
    
    // Kennung ok, die Daten wurden durch einen vorherigen Vorgang erzeugt    
  } else {    
    
    debugln("Letzte Wiegung nicht gefunden.");
    strcpy(rtc.check, "WFW2");  
     
    rtc.starttime = 0;
    rtc.timestamp = 0;
    rtc.lastweight=0; 
    rtc.lasttransfer=0;
	  rtc.sleeptime=0;
    rtc.lastweightok=false; // Dies macht eine Übertragung erforderlich (direkt nach Neustart)  
  }  
  return(rtc);
}


void writertc(rtcmem rtc) {              
    rtc.timestamp = rtc.starttime+millis()/1000;    
    
    debugln("Wiegung wird im RTC Speicher abgelegt");
    debug("Zeit:   ");
    debugln(rtc.timestamp);
    debug("Gewicht: ");
    debugln(rtc.lastweight);
       
    
    ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtc, sizeof(rtc));    
}  


void process_scale() {
  // Wiegung durchfuehren    
  
  hx711result ret = getweight(scale, configdata.weightavgcount, configdata.minmaxthreshold, configdata.weightavgmaxtry, 5000);
  scale.power_down();   
  supply_power=(float)ESP.getVcc()/1000.0;
  delay(50);   

  
  debug("Wiegung erfolgt: ");
  debug("Rohgewicht:  ");
  debugln(ret.weight);
  debug("Iterationen: ");
  debugln(ret.iterations);
    

  float weight = ret.weight/configdata.scale_factor-configdata.scale_tara;    
  
  bool transmitdata=false;
  int transmitreason=0;

  
  debug("Zeitdifferenz seit letzter Übertragung ");
  debugln(rtc.starttime-rtc.lasttransfer);  
  debug("Konfiguration=");
  debugln(configdata.heartbeat);
  


  // Zeit für regelmäßige Meldung abgelaufen?
  if (((float)(rtc.starttime-rtc.lasttransfer)/60)>=(float)configdata.heartbeat) {
    transmitdata=true;
    transmitreason=1; // Heartbeat
  }
  
  // Differenz alter/neuer Wert?
  if (rtc.lastweightok & abs(weight-rtc.lastweight)>=configdata.thresholdmin & abs(weight-rtc.lastweight)<=configdata.thresholdmax) {
    if (ret.status==0 | configdata.transmitincomplete) {
      transmitdata=true;
      transmitreason=2; // Gewichtsänderung
    }
  }
  
  if (!rtc.lastweightok) {
    transmitdata=true;
    transmitreason=3; // Beim ersten Start Übertragung auslösen
  }

  
  debug("Transmit reason: ");
  debugln(transmitreason);
  

  if (transmitdata) {    
    // Wert soll übertragen werden
    WiFi.forceSleepWake();  
    delay(1);
    startWifi();    // WiFi verbinden und Zeit synchronisieren
    // time
    time_t now = time(nullptr);   
    long timetrack = rtc.starttime+millis()/1000;
    long timetrackerror = timetrack - (long)now;
    
    debug("Timetracking: berechnet=");    
    debug(timetrack);    
    debug(", NTP=");
    debug((long)now);
    debug(", Fehler (Sekunden)=");
    debugln(timetrackerror);
        
    
    rtc.starttime = (long)now-millis()/1000; // Startzeit korrigieren!    
    struct tm *timeptr=localtime(&now);  
    char buff[32];
    strftime(buff,sizeof(buff),"%Y-%m-%d %H:%M:%S",timeptr); 

    
    debug("connecting to ftp site at: ");  
    debugln(configdata.ftp_host);
      
  
    String data="";     
    float diffweight=0;

    if (rtc.lastweightok) {
      diffweight=weight-rtc.lastweight;
    } else {
      diffweight=0;
      timetrackerror=0;
    }
    if (abs(diffweight)<configdata.thresholdmin) diffweight=0;

    supply_power=((float)ESP.getVcc())/1000.0;  // Spannungswert aktualisieren
    data += String(buff); // Datum und Uhrzeit
    data += ",";
    data += String(diffweight,0);  // Gewichtsdifferenz (in Gramm nach Kalibrierung)
    data += ",";
    data += String(weight,0);  // Gewicht in Gramm nach Kalibrierung
    data += ",";
    data += String(ret.weight,0); // Rohwert der Gewichtsmessung für spätere Kontrolle
    data += ",";
    data += String(ret.iterations); // Anzahl Messungen
    data += ","; 
    data += String(ret.status);  // Status der Messung
    data += ","; 
    data += String(WiFi.RSSI());  // Signalstärke der WLAN-Verbindung
    data += ","; 
    data += String(supply_power); // Spannung am ESP
//    data += ",";
//    data += String(timetrackerror);
//    data += ",";
//    data += String(transmitreason);
   
    
    debug("data: \"");    
    debug(data);
    debugln("\"");
      
    
    bool ok = log_data(data);
    if (ok) {
      rtc.lastweightok=true;
      rtc.lasttransfer = (long)now;
    }

    
    if (ok) {
      debugln("Daten wurden erfolgreich an FTP Server gesendet");
    } else {
      debugln("Fehler beim Senden der Daten an den FTP Server");
    } 
     
    delay(5);    
  	WiFi.disconnect(true);
  	WiFi.forceSleepBegin();
  	delay(5);
  } else {    
    debugln("Keine Übertragung erforderlich.");    
  }
  if (ret.status==0) {
      // bestätigtes Gewicht als letztes Gewicht übernehmen
      // wenn keine stabile Messung zustande gekommen ist, wird das alte Gewicht weiter genutzt
      rtc.lastweight=weight;
    }   
}

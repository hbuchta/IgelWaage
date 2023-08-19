// https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
// https://randomnerdtutorials.com/install-esp8266-nodemcu-littlefs-arduino/
// https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html


// https://www.w3schools.com/html/html_table_borders.asp
const String html_intro = R"====(
<html>
<head>
<style>
  input,select,td,th,h1,a,p,font {font-family: Arial; font-size: 12px;}
  table, th, td { border: 1px solid white; border-collapse: collapse;}
  th {background-color: #a0a0a0;}
  td {background-color: #c0c0c0;}
</style>
<meta name="viewport" content="width=device-width, initial-scale=1.0"/> 
</head>
<body>
)====";

const String password_notmodified = "§§NOTMODIFIED§§";


ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'



String html_encode(String s) {
  String r=s;
  r.replace("&","&amp;");
  r.replace("\"","&quot;"); 
  r.replace("'","&#039;"); 
  r.replace("<","&lt;");
  r.replace(">","&gt;");
  return(r);
}

String html_decode(String s) {
  String r=s;
  r.replace("&quot;","\"");  
  r.replace("&amp;","&");
  r.replace("&lt;","<");
  r.replace("&gt;",">");
  r.replace("&#039;","'"); 
  return(r);
}


void checkconfig(eeromstruct &configdata, configstruct &webconfig) {
  webconfig.wifi_ok =
    (String(configdata.wifi_ssid).length()>0)
    & (String(configdata.ntp_server).length()>0);

  webconfig.scale_ok =
    (configdata.scale_factor!=0) & (configdata.scale_tara!=0);

  webconfig.scale_factor = configdata.scale_factor;
  webconfig.scale_tara = configdata.scale_tara;

  webconfig.ftp_ok =
    (String(configdata.ftp_host).length()>0)
    & (String(configdata.ftp_user).length()>0)
    & (String(configdata.ftp_path).length()>0);    

  webconfig.options_ok=true;  

  webconfig.all_ok = webconfig.wifi_ok & webconfig.ftp_ok & webconfig.scale_ok & webconfig.options_ok;
}

bool load_config(eeromstruct &configdata) {
  EEPROM.get(0,configdata);  
 
  if (String(configdata.check)!="WIFISCALE") {  
        
    debugln("Keine Konfiguration hinterlegt");
    debug("Check:");
    debugln(String(configdata.check));
    
    
    // Konfiguration neu anlegen
    configdata.version=wifiscale_version;    
    
    strcpy(configdata.check,"WIFISCALE");   

    // WiFi
    strcpy(configdata.wifi_ssid, "");  
    strcpy(configdata.wifi_pwd, "");  

    // FTP
    strcpy(configdata.ftp_host,"");   
    strcpy(configdata.ftp_user,"");   
    strcpy(configdata.ftp_pwd,"");   
    strcpy(configdata.ftp_path,"");
    configdata.ftp_port=21;

    // Kalibrierung
    configdata.scale_factor=0;
    configdata.scale_tara=0;    

    // Options
    configdata.thresholdmin=50;
    configdata.thresholdmin=2500;
    configdata.weightavgcount=15;  
    configdata.weightavgmaxtry=100;
    configdata.minmaxthreshold=200;
    configdata.transmitincomplete=true;
    
    configdata.heartbeat=60;  // Minuten
    configdata.sleeptime=90;  // Sekunden
    strcpy(configdata.ntp_server,"pool.ntp.org");   

    return(false);
 } else {
  configdata.version=wifiscale_version;
  strcpy(configdata.release_version, release_version);
  return(true);
 }
}



void start_config_server(){

  // Access Point starten

  String wifiname = "WifiScale-";
  wifiname += String(ESP.getChipId(),HEX);

  WiFi.hostname("wifiscale");

  IPAddress local_IP(192,168,1,4);
  WiFi.softAPConfig(local_IP, local_IP, IPAddress(255, 255, 255, 0));

  boolean result = WiFi.softAP(wifiname, "hedgehog");

  dnsServer.start(53, "*", local_IP);  // DNS Server auf Port 53, alle Adressen (*) => eigene IP-Adresse  

  if(result == true)
  {    
    
    debug("WiFi Access Point gestartet, IP address: ");
    debug(WiFi.softAPIP());    
    debug(", MAC Adresse: ");
    debugln(WiFi.softAPmacAddress());   
    

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  
    // https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
    // https://ullisroboterseite.de/esp8266-webserver-klasse.html, HTTP_GET
    server.on("/", handleRoot);                 // Call the 'handleRoot' function when a client requests URI "/"          
    server.on("/wificonfig",handleWiFiConfig);  
    server.on("/calibration",handleCalibration);  
    server.on("/scaletest",handleScaleTest);
    server.on("/ftpconfig", handleFtpConfig);
    server.on("/options", handleOptions);
    server.on("/saveconfig", handleSaveConfig);
    server.on("/connectiontest", handleConnectionTest);
	  server.on("/resetconfig", handleResetConfig);
    
    server.onNotFound(handleNotFound);          // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  
    server.begin();                           // Actually start the server
    debugln("HTTP Server für Konfiguration gestartet");
    debug("Verbinde dich jetzt mit dem WLAN ");
    debug(wifiname);
    debugln(" und öffne dann");
    debugln("die Seite www.wifiscale.org oder 192.168.4.1 im Browser");
    
    if (load_config(configdata)) {   
      webconfig.scale_factor=configdata.scale_factor;
      webconfig.scale_tara=configdata.scale_tara;
    } else {
      
      debugln("Konfiguration nicht gefunden");
              
    }
    checkconfig(configdata, webconfig);
  }
  else
  {
    
    debugln("Webserver konnte nicht gestartet werden!");
      
  }  
}



void handleRoot() { 
  
  String html = html_intro;
  html+="<h1>WiFi Waage Einstellungen</h1><hr>\n";
  html+="<p><i>Version "+html_encode(String(configdata.release_version))+"</i></p>\n";
  
  html+="<table>\n";

  html+="<tr><td>";
  html+= (webconfig.wifi_ok?"<font color=\"#008000\">OK</font>":"<font color=\"#ff0000\">todo</font");
  html+="</td><td>WiFi Konfiguration</td><td><a href=\"/wificonfig\">&auml;ndern</a></td></tr>\n";      

  html+="<tr><td>";
  html+= (webconfig.ftp_ok?"<font color=\"#008000\">OK</font>":"<font color=\"#ff0000\">todo</font");
  html+="</td><td>FTP-Server Konfiguration</td><td><a href=\"/ftpconfig\">&auml;ndern</a></td></tr>\n";      

  html+="<tr><td>";
  html+= (webconfig.scale_ok?"<font color=\"#008000\">OK</font>":"<font color=\"#ff0000\">todo</font");
  html+="</td><td>Kalibrierung der Waage</td><td><a href=\"/calibration\">&auml;ndern</a>";
  if (webconfig.scale_ok) html+=" | <a href=\"/scaletest\">Test</a>";
  html+="</td></tr>\n";

  html+="<tr><td>";
  html+= (webconfig.options_ok?"<font color=\"#008000\">OK</font>":"<font color=\"#ff0000\">todo</font");
  html+="</td><td>Optionale Einstellungen</td><td><a href=\"/options\">&auml;ndern</a>";  
  html+="</td></tr>\n";
  html+= "</table>\n";

  html+="<hr>\n";

  if (webconfig.all_ok) {    
   if (webconfig.modified) {
    html+="<p><font color=\"red\"><i>Konfiguration ist vollst&auml;ndig, aber noch nicht gespeichert</i></font></p>";
   } else {
    html+="<p><font><i>Konfiguration ist vollst&auml;ndig</i></font></p>";
   }   
  } else {
    html+="<p><font color=\"red\"><i>Bitte vervollst&auml;ndigen Sie die Konfigurationseinstellungen</i></font></p>";
  }
  html+="\n<div>";
  if (webconfig.modified) {
	html+="<input type=\"button\" value=\"Einstellungen speichern\" onClick=\"window.location.href='/saveconfig'\" style=\"margin-right:8px\"/>";
	html+="<input type=\"button\" value=\"Konfiguration zur&uuml;cksetzen\" onClick=\"if (confirm('Sind Sie Sicher, dass die Ihre Einstellungen zur&uuml;cksetzen wollen?') {window.location.href='/resetconfig'; }\" style=\"margin-right:8px\"/>";
  }
  if (webconfig.wifi_ok & webconfig.ftp_ok) {
    html+="<input type=\"button\" value=\"Verbindungstest\" onClick=\"window.location.href='/connectiontest'\"/>";
  }    
  html+="</div>\n</body>\n</html>\n";   
    
  server.send(200, "text/html", html);   // Send HTTP status 200 (Ok) and send some text to the browser/client  
  server.client().stop();
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
}


void handleWiFiConfig() {
  String error="";
  if (server.method()==HTTP_POST) {
    if (!server.hasArg("ssid") || !server.hasArg("pwd")) {
      server.send(400,"text/plain","400: Invalid Request");
      return;
    }
    String ssid=server.arg("ssid");
    ssid.trim();           
    String ntp=server.arg("ntp");
    ntp.trim();
    String pwd=server.arg("pwd");
    pwd.trim();
    if (ssid.length()==0) {
      error="SSID darf nicht leer sein";
    } else if (ntp.length()==0) {
      error="NTP-Server darf nicht leer sein";
    } else {
      // redirect    
      ssid.toCharArray(configdata.wifi_ssid, sizeof(configdata.wifi_ssid));
      ntp.toCharArray(configdata.ntp_server, sizeof(configdata.ntp_server));
  
      if (!pwd.equals(password_notmodified)) pwd.toCharArray(configdata.wifi_pwd, sizeof(configdata.wifi_pwd));    
      //server.arg("pwd").toCharArray(configdata.wifi_pwd, sizeof(configdata.wifi_pwd));
      
      webconfig.wifi_ok=true;  
      webconfig.modified=true;    
      server.sendHeader("Location", String("/"), true);
      server.send ( 302, "text/plain", "");
      return;
    }
  }
  
  String html=html_intro;

  html+="<h1>WLAN Einstellungen</h1><hr>";
  
  html += "<form method=\"post\" action=\"/wificonfig\" name=\"wificonfig\">";
  if (error.length()>0) {
    html+="<font color=\"#FF0000\">"+error+"</font><br>";
  }
  html += "<table>\n";
  // https://developer.mozilla.org/de/docs/Web/HTML/Element/Input
  html += "<tr><td>WLAN SSID</td><td><input name=\"ssid\" type=\"text\" maxlength=80 ";
  html += "value=\"" + html_encode(String(configdata.wifi_ssid)) + "\"";
  html += "</td></tr>\n";
  html += "<tr><td>WLAN Password</td><td><input name=\"pwd\" type=\"password\" maxlength=80 ";
  html += "value=\"" + password_notmodified + "\"";
  html += "</td></tr>\n";
  html += "<tr><td>NTP-Server</td><td><input name=\"ntp\" type=\"text\" maxlength=80 ";
  html += "value=\"" + html_encode(String(configdata.ntp_server)) + "\"";
  html += "</td></tr>\n";

  
  html += "</table>\n";
  // https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input/button
  
  html+="<hr>";
  html += "<br><input type=\"button\" value=\"Abbruch\" onClick=\"window.location.href='/'\"/>";
  html += "<input type=\"submit\" value=\"&Uuml;bernehmen\"></input>";
  html += "</form>";
  html += "</body></html>";
  
  server.send(200, "text/html", html); 
  server.client().stop();    
}

boolean isValidNumber(String str) {
  for(byte i=0;i<str.length();i++)
  {
    if(!isDigit(str.charAt(i))) return false;
  }
  return true;
}

void handleCalibration() {
  int step=0;
  String error="";  

  // step
  // 0: Introseite
  // 1: Erste Messung (Referenz 0 Gramm) abgeschlossen
  // 2: Zweite Messung (Referenzgewicht 1) abgeschlossen
  // 3: Dritte Messung (Referenzgewicht 2) abgeschlossen, Ergebnis anzeigen, final bestätigen  
  // 4: Bestätigt, Speichern und zurück zur Hauptseite
  
  if (server.method()==HTTP_GET) {
    webconfig.weight=0;
  } else if (server.method()==HTTP_POST) {
    if (!server.hasArg("step")) {
      server.send(400,"text/plain","400: Invalid Request");
      return;
    }
    step=server.arg("step").toInt();

    if (step==4) {
      webconfig.scale_ok=true;      
      configdata.scale_factor=webconfig.scale_factor;
      configdata.scale_tara=webconfig.scale_tara;      
      webconfig.modified=true;
      server.sendHeader("Location", String("/"), true);
      server.send(302,"text/plain", "");
      return;
    }

    if (step>1 & step<4) {
      if (!server.hasArg("weight")) {    
        server.send(400,"text/plain","400: Invalid Request");
        return;
      }
      String inputweight=server.arg("weight");
      inputweight.trim();
      if (inputweight.length()==0 | !isValidNumber(inputweight)) {
        step--;
        error="Gewicht muss eine Zahl in Gramm sein (keine Sonderzeichen)";
      } else {      
        webconfig.weight=server.arg("weight").toFloat();    
      }
    }  
  }

  float measure;
  if (step>0) {
    // Messung durchführen
    hx711result ret = getweight(scale);
    measure = ret.weight;
    if (ret.status>0) {
      error="Fehler bei der Gewichtsmessung. Stellen Sie sicher, dass die Waage korrekt angeschlossen ist, das Gewicht aufliegt und die Waage in Ruhe ist.";
      step--;
    }
  }
  String html = html_intro;

  html+="<h1>Kalibrierung der Waage</h1><hr>";    
  
  html += "<form method=\"post\" action=\"/calibration\" name=\"scaleconfig\">";

  if (step>0 & step<4) {
    webconfig.scale[step-1]=measure;
    webconfig.scalecalib[step-1]=webconfig.weight;    
   }
 
  if (step==0) {
    html+="<p>Zur Kalibrierung der Waage werden nacheinander drei Messungen durchgef&uuml;hrt.<br>";
    html+="Die erste Messung erfolgt auf der leeren Waage (keine Gewichte auf den vier Messpunkten.<br>";
    html+="Die zwei und dritte Messung erfolgt, indem Sie ein bekanntes Gewicht auf die Waage stellen<br>";
    html+="und dieses Gewicht erfassen. Ideal sind hierf&uuml;r Gewichte von etwa z.B. 2kg und 4kg<br>";
    html+="Bitte denken Sie daran, dass sie auch die Platte, auf die Sie das Gewicht stellen mit<br>";
    html+="erfassen<br><br>";
    html+="Wir beginnen ohne Gewicht. Wenn die Waage aufgestellt ist und kein Gewicht auf den Wiegepunkten<br>";
    html+="steht, dr&uuml;cken Sie auf weiter.<p><br>";
  }
  if (step>0 & step<3) {
    if (step==1) html+="<p>Gut gemacht. Legen Sie nun das erste Gewicht auf die Waage ";
    if (step==2) html+="<p>Fast geschafft. Legen Sie nun das zweite Gewicht auf die Waage";
    html+="und geben Sie<br>in das folgende Feld das Gewicht in Gramm (ohne Kommastellen oder Tausenderpunkt) ein.</p><br>";
    html +="<table>\n";
    html +="<tr><td>Aufgelegtes Gewicht (Gramm)</td>\n<td>";
    html+="<input name=\"weight\"";  
    if (error.length()>0) {
      // Letzten eingegebenen Wert im Fehlerfall wieder übernehmen
      html+=" value=\"";
      html+=html_encode(String(webconfig.weight));
      html+="\"";
    }
    html+="/>"; // Ende vom Input-Tag    
    html += "</td>\n</tr></table>\n";
  }

  if (step>=3) {
    webconfig.scale_factor=(
      (webconfig.scale[2]-webconfig.scale[0]) / (webconfig.scalecalib[2]-webconfig.scalecalib[0])
      + (webconfig.scale[1]-webconfig.scale[0]) / (webconfig.scalecalib[1]-webconfig.scalecalib[0])
      + (webconfig.scale[2]-webconfig.scale[1]) / (webconfig.scalecalib[2]-webconfig.scalecalib[1])
      ) / 3.0;
    webconfig.scale_tara=webconfig.scale[0] / webconfig.scale_factor;

    html+="\n\n<p>Berechnete Werte</p>";
    html+="<table><tr><td>Faktor</td><td>";
    html+=String(webconfig.scale_factor);
    html+="</td></tr>\n<tr><td>Nullstellung</td><td>";
    html+=String(webconfig.scale_tara);
    html+="</td></tr>\n</table>\n";

    float newval[3];
    for (int i=0; i<3;i++) {
      newval[i] = webconfig.scale[i]/webconfig.scale_factor-webconfig.scale_tara;
    }

    html +="<table>\n";
    html +="<tr><th>Aufgelegtes Gewicht</th><th>Gemessener Wert</th><th>Gemessener Wert nach Korrektur</th><tr>\n";        
    for (int i=0;i<3;i++) {
      html+="<tr><td>";
      html+=String(webconfig.scalecalib[i]);
      html+="</td><td>";
      html+=String(webconfig.scale[i]);
      html+="</td><td>";
      html+=String(newval[i]);
      html+="</td></tr>\n";
    }
    html += "</table><br>\n";
    
    float mean_error = (
      abs((newval[2]-newval[0])-(webconfig.scalecalib[2]-webconfig.scalecalib[0]))
      + abs((newval[1]-newval[0])-(webconfig.scalecalib[1]-webconfig.scalecalib[0]))
      + abs((newval[2]-newval[1])-(webconfig.scalecalib[2]-webconfig.scalecalib[1]))
    ) / 3.0;

    html+="<p>Mittlerer Fehler = ";
    html+=String(mean_error);
    html+=" Gramm</p>";
    
    if (mean_error>30) {
      html+="<font color=\"#ff0000\"><b>Es wird empfohlen, die Kalibrierung zu wiederholen!</b></font><br>";
    } else {
      html+="<font color=\"#008000\">Werte sind ok, Ergebnis kann &uuml;bernommen werden.</font><br>";
    }   
  }

  html += "<input name=\"step\" type=\"hidden\" value=\""+String(step+1)+"\"/>";

  html+="<hr>";
  if (step>=3) {
    html += "<br><input type=\"button\" value=\"Erneut kalibrieren\" onClick=\"window.location.href='/calibration'\"\>";
    html += "<input  type=\"submit\" value=\"Werte &uuml;bernehmen\"></input>";
  } else {
    html += "<br><input type=\"button\" value=\"Abbruch\" onClick=\"window.location.href='/'\"\>";
    html += "<input  type=\"submit\" value=\"Weiter\"></input>";
  }
  html +="</form>"; 

  html+="</body>\n</html>\n";
  server.send(200, "text/html", html); 
  server.client().stop();      
}



void handleScaleTest() {
  if (server.hasArg("reset")) {   
    webconfig.last_test_weight=0;
  }  
  
  String html = html_intro;
  html+="<h1>Test der Waage</h1><hr>";    

  html+="\n\n<p>Kalibrierungsparameter:</p>";
  html+="<table><tr><td>Faktor</td><td>";
  html+=String(webconfig.scale_factor);
  html+="</td></tr>\n<tr><td>Nullstellung</td><td>";
  html+=String(webconfig.scale_tara);
  html+="</td></tr>\n</table>\n";

  html+="<hr>";

  float measure;  
  hx711result ret = getweight(scale);
  measure = ret.weight;
  measure = measure / configdata.scale_factor-configdata.scale_tara;
  if (ret.status>0) {
    html+="<font color=\"red\"><b>Fehler bei der Gewichtsmessung. Stellen Sie sicher, dass die Waage korrekt angeschlossen ist, das Gewicht aufliegt und die Waage in Ruhe ist.</b></font><br>\n";        
  }  else {
    html+="<table>\n";
    html+="<tr><td>Aktuelle Messung</td><td>";
    html+=String(measure);
    html+="</td></tr>\n";
    html+="<tr><td>Rohwert</td><td>";
    html+=String(ret.weight);
    html+="</td></tr>\n";
    if (webconfig.last_test_weight!=0) {
      html+="<tr><td>Letzte Messung</td><td>";
      html+=String(webconfig.last_test_weight);
      html+="</td></tr>\n";
      html+="<tr><td>Differenz</td><td>";
      html+=String(measure-webconfig.last_test_weight);
      html+="</td></tr>\n";
      html+="</table>\n";      
    }
    webconfig.last_test_weight = measure;
  }
  
  html+="<input type=\"button\" value=\"Zur&uuml;ck\" onClick=\"window.location.href='/'\">";
  html+="<input type=\"button\" value=\"Erneut messen\" onClick=\"window.location.href='/scaletest'\">";  // Alternativ: onlClick='refresh'

  server.send(200, "text/html", html);   
}


void handleFtpConfig() {
  String error="";

  if (server.method()==HTTP_GET) {
    // Einstellungen anzeigen  
  } else if (server.method()==HTTP_POST) {
    // Werte speichern und zurück
     if (!server.hasArg("ftphost") | !server.hasArg("ftppass") | !server.hasArg("ftpuser")) {
        server.send(400,"text/plain","400: Invalid Request");
        return;
      } 
      
      String par_host=server.arg("ftphost");
      String par_port=server.arg("ftpport");
      String par_user=server.arg("ftpuser");
      String par_pass=server.arg("ftppass");
      String par_path=server.arg("ftppath");
      
      par_host.trim();
      par_port.trim();
      par_user.trim();
      par_pass.trim();
      par_path.trim();

      if (par_host.length()>0 & par_port.length()>0 & par_user.length()>0 & par_pass.length()>0) {
        par_host.toCharArray(configdata.ftp_host, sizeof(configdata.ftp_host)); 
        configdata.ftp_port = par_port.toInt();                     
        par_user.toCharArray(configdata.ftp_user, sizeof(configdata.ftp_user));
        if (!par_pass.equals(password_notmodified)) par_pass.toCharArray(configdata.ftp_pwd, sizeof(configdata.ftp_pwd));
        par_path.toCharArray(configdata.ftp_path, sizeof(configdata.ftp_path));
        
        webconfig.ftp_ok=true; 
        webconfig.modified=true;     
        server.sendHeader("Location", String("/"), true);
        server.send ( 302, "text/plain", "");
        return;  
      } else {
        error="Bitte f&uuml;llen Sie alle Felder aus.";
      }
  }

  String html = html_intro;
  html+="<h1>Optionen</h1><hr>";    
  
  html += "<form method=\"post\" action=\"/ftpconfig\" name=\"ftpconfig\">";

  if (error.length()>0) html+="<p><font color=\"#FF0000\">"+error+"</font></p>";
  
  html+="<table><tr><th>Einstellung</th><th>Wert</th><th>Beschreibung</th></tr>";
  
  html+="<tr><td>FTP-Server</td><td><input name=\"ftphost\" type=\"text\" value=\""+html_encode(String(configdata.ftp_host))+"\" maxlength=80></td>";
  html+="<td>Name oder IP-Adresse des FTP-Servers im Heimetz</td><tr>";
  
  html+="<tr><td>Port</td><td><input name=\"ftpport\" type=\"number\" value=\""+html_encode(String(configdata.ftp_port))+"\"></td>";
  html+="<td>Port des FTP-Servers, normalerweise 21</td><tr>";
  
  html+="<tr><td>FTP User</td><td><input name=\"ftpuser\" type=\"text\" value=\""+html_encode(String(configdata.ftp_user))+"\" maxlength=80></td>";
  html+="<td>Benutzername f&uuml; den FTP-Server</td><tr>";

  html+="<tr><td>FTP Kennwort</td><td><input name=\"ftppass\" type=\"password\" value=\""+password_notmodified+"\" maxlength=80></td>";
  html+="<td>Kennwort f&uuml;r den FTP-Server</td><tr>";

  html+="<tr><td>Verzeichnis</td><td><input name=\"ftppath\" type=\"text\" value=\""+html_encode(String(configdata.ftp_path))+"\" maxlength=80></td>";
  html+="<td>Verzeichnis f&uuml;r Protokolldateien auf dem FTP-Server. Dieses Verzeichnis muss existieren!</td><tr>";

  html+="</table>";
  
  html+="<hr>";
  html+="<input type=\"button\" value=\"Abrechen\" onClick=\"window.location.href='/'\">";
  html+="<input type=\"submit\" value=\"Speichern\">";  
  
  html+="</body>\n</html>";
  server.send(200, "text/html", html);   
}




void handleOptions() {
  String error="";
  if (server.method()==HTTP_GET) {
    // Einstellungen anzeigen  
  } else if (server.method()==HTTP_POST) {
    // Werte speichern und zurück     
      
      String par_sleeptime=server.arg("sleeptime");
      String par_thresholdmin=server.arg("thresholdmin");
      String par_thresholdmax=server.arg("thresholdmax");
      String par_heartbeat=server.arg("heartbeat");      
      String par_weightavgcount=server.arg("weightavgcount");
      String par_weightavgmaxtry=server.arg("weightavgmaxtry");     
      String par_minmaxthreshold=server.arg("minmaxthreshold");      
      String par_transmitincomplete=server.arg("transmitincomplete");
      
      par_sleeptime.trim();
      par_thresholdmin.trim();
      par_heartbeat.trim();
      par_weightavgcount.trim();
      par_weightavgmaxtry.trim();
      par_transmitincomplete.trim();
      par_minmaxthreshold.trim();
      
      if (par_sleeptime.length()>0 & par_thresholdmin.length()>0 & par_thresholdmax.length()>0 & par_heartbeat.length()>0 & par_weightavgcount.length()>0 & par_weightavgmaxtry.length()>0 & par_minmaxthreshold.length()>0 ) {
        configdata.sleeptime = par_sleeptime.toInt();
        configdata.thresholdmin = par_thresholdmin.toFloat();
        configdata.thresholdmax = par_thresholdmax.toFloat();        
        configdata.heartbeat = par_heartbeat.toFloat();
        configdata.weightavgcount=par_weightavgcount.toInt();
        configdata.weightavgmaxtry=par_weightavgmaxtry.toInt();
        configdata.minmaxthreshold=par_minmaxthreshold.toInt();
        configdata.transmitincomplete=(par_transmitincomplete=="1");
        
        webconfig.options_ok=true;  
        webconfig.modified=true;    
        server.sendHeader("Location", String("/"), true);
        server.send ( 302, "text/plain", "");
        return;  
      } else {
        error="Bitte f&uuml;llen Sie alle Felder aus.";
      }
  }

  String html = html_intro;
  html+="<h1>Optionen</h1><hr>";    
  
  html += "<form method=\"post\" action=\"/options\" name=\"options\">";

  if (error.length()>0) html+="<p><font color=\"#FF0000\">"+error+"</font></p>";
  html+="<table><tr><th>Einstellung</th><th>Wert</th><th>Beschreibung</th></tr>";
  
  html+="<tr><td>Sleeptime</td><td><input name=\"sleeptime\" type=\"number\" value=\""+html_encode(String(configdata.sleeptime)) +"\"> Sekunden</td>";
  html+="<td>Wartezeit bis zur n&auml;chsten Messung (in Sekunden)</td><tr>";
  
  html+="<tr><td>Schwellwert Min.</td><td><input name=\"thresholdmin\" type=\"number\" maxlength=\"6\" size=\"6\" value=\""+ html_encode(String(configdata.thresholdmin,0)) +"\"> Gramm</td>";
  html+="<td>Eine &Uuml;bertragung findet bei &Uuml;berschreitung dieses Wertes statt (in Gramm)</td><tr>";
  html+="<tr><td>Schwellwert Max.</td><td><input name=\"thresholdmax\" type=\"number\" maxlength=\"6\" size=\"6\" value=\""+ html_encode(String(configdata.thresholdmax,0)) +"\"> Gramm</td>";
  html+="<td>Eine &Uuml;bertragung findet nur bis zum diesem Wert (in Gramm) statt</td><tr>";

  html+="<tr><td>Lebenszeichen</td><td><input name=\"heartbeat\" type=\"number\" maxlength=\"6\" size=\"6\" value=\""+ html_encode(String(configdata.heartbeat)) +"\"> Minuten</td>";
  html+="<td>Auch ohne &Uuml;berschreitung des Schwellwertes findet eine &Uuml;bertragung etwa nach dieser Zeit statt (in Minuten)</td><tr>";

  html+="<tr><td>Anz. Wiegungen</td><td><input name=\"weightavgcount\" type=\"number\" maxlength=\"6\" size=\"6\" value=\""+html_encode(String(configdata.weightavgcount)) +"\"> Wiegungen</td>";
  html+="<td>Anzahl Wiegungen, die zu einem Ergebnis gemittelt werden.</td><tr>";

  html+="<tr><td>Max. Differenz</td><td><input name=\"minmaxthreshold\" type=\"number\" maxlength=\"6\" size=\"6\" value=\""+html_encode(String(configdata.minmaxthreshold)) +"\"> Gramm</td>";
  html+="<td>Wenn w&auml;hrende der Wiegungen die Differenz Max-Min gr&ouml;&szlig;er als dieser Wert ist, beginnt die Messung erneut.</td><tr>";

  html+="<tr><td>Max. Anzahl Wiegungen</td><td><input name=\"weightavgmaxtry\" type=\"number\" maxlength=\"6\" size=\"6\" value=\""+html_encode(String(configdata.weightavgmaxtry)) +"\"> Wiegungen</td>";
  html+="<td>Maximale Anzahl an Wiegungen, die versucht werden, bevor ein instabiles Ergebnis ermittelt wird.</td><tr>";

  html+="<tr><td>Instabiles Ergebnis senden</td><td><input name=\"transmitincomplete\" type=\"checkbox\" value=\"1\"" + String(((configdata.transmitincomplete)?" checked":"")) +"></td>";
  html+="<td>Soll ein instabiles Ergebnis &uuml;bertragen werden?</td><tr>";

  html+="</table>";

  html+="<hr>";
  html+="<input type=\"button\" value=\"Abrechen\" onClick=\"window.location.href='/'\">";
  html+="<input type=\"submit\" value=\"Speichern\">";
  
  html+="</body>\n</html>";
  server.send(200, "text/html", html);   
}



void handleSaveConfig() {
   
   debugln("Konfiguration wird gespeichert");
   debugln(configdata.check);
   debugln(configdata.version);   
   
   EEPROM.put(0,configdata);   
   EEPROM.commit();   
   delay(10);
   webconfig.modified=false;          
   server.sendHeader("Location", String("/"), true);
   server.send (302, "text/plain", "");   
}


void handleResetConfig() {
      
   debugln(configdata.check);
   debugln(configdata.version);   
   
   load_config(configdata);
   checkconfig(configdata, webconfig);
   server.sendHeader("Location", String("/"), true);
   server.send (302, "text/plain", "");   
}


void handleConnectionTest() {
  bool ok=true;
  const int max_loop=10;
  int l;
  
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    
  // here begin chunked transfer


  String html = html_intro;
  html+="<h1>Optionen</h1><hr>\n<pre>Verbindungstest zu WLAN SSID ";   
  html+=configdata.wifi_ssid;
  html+=": ";
  //server.sendContent(html);
  server.send(200, "text/html", html);  
  delay(500);

  // WiFi Verbindung aufbauen
  l=0;
  WiFi.begin(configdata.wifi_ssid, configdata.wifi_pwd);
  while (WiFi.status() != WL_CONNECTED) {
    if (l>max_loop) break;
    server.sendContent("*");    
    delay(500);
  } 
  delay(500);
  if (l>max_loop) {
    ok=false;
    html = "\nTimeout, Verbindung konnte nicht hergestellt werden.";
    server.sendContent(html); 
    delay(500);
  } else {
    html = "\nVerbindung hergestellt, IP Adresse:";
    html += WiFi.localIP().toString();
    html+="\n\nVerbindung zu NTP-Server ";
    html+=configdata.ntp_server;
    html+=": ";
    server.sendContent(html); 
    delay(500);
    
    configTime(3 * 3600, 0, configdata.ntp_server);  
    time_t now = time(nullptr);
    l=0;
    while (now < 8 * 3600 * 2) {
      if (l>max_loop) break;
      delay(500);      
      server.sendContent("*");      
      now = time(nullptr);      
    }   
    if (l>max_loop) {
      ok=false;
      html = "\nVerbindung konnte nicht hergestellt werden.";
      server.sendContent(html); 
      yield();     
    } else {
      html = "\nVerbindung hergestellt, Uhrzeit:";
      struct tm *timeptr=localtime(&now);  
      char buff[32];
      strftime(buff,sizeof(buff),"%Y-%m-%d %H:%M:%S",timeptr);
      html+=String(buff);
      html+="\n\nVerbindung zu FTP-Server ";
      html+=configdata.ftp_host;
      html+=": ";
      server.sendContent(html);
      delay(500);

      String ftp_result=check_ftp();
      if (ftp_result=="ok") {
        html="FTP Verbindungstest erfolgreich.";
      } else {
        ok=false;
        html="FTP Verbindungstest gescheitert: ";
        html += ftp_result;
      }
      server.sendContent(html);
      delay(500);
    }    
  }
  html="\n</pre><br>";
  html += ok?"Verbindungstest erfolgreich":"<font color=\"red\">Verbindungstest fehlgeschlagen.</font>";
  
  html+="<br><input type=\"button\" value=\"Zur&uuml;ck\" onClick=\"window.location.href='/'\">";

  html+="</body>\n</html>";
  server.sendContent(html); 
  server.sendContent(""); // this tells web client that transfer is done 
  yield();
  
  server.client().stop();    
}

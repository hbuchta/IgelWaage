////////////////////////////////////////////
// FTP-Verbindung zum Wegschreiben der Ergebnisse

String get_response(WiFiClient client) {
  String result;
  result.reserve(2048);
  char buf[256]; 
  
  unsigned long timeout = millis();
  while (!client.available()) {
    if (millis() - timeout > 5000) {
      debugln(">>> Client Timeout !");
      client.stop();
      return(result);
    }
  }
 
  while (client.available()) {
    size_t len = client.readBytes(buf, sizeof(buf));
    result.concat(String(buf).substring(0, len));
  }
  
  return(result);
}

void send_string(WiFiClient client, String s) {    
  if (client.connected()) {
    client.println(s);
  }
}


String check_ftp() {
  WiFiClient client;
  String s;
  if (!client.connect(configdata.ftp_host, configdata.ftp_port)) {      
    return("Server hat nicht geantwortet");
  }
  s=get_response(client);    
  if (!s.startsWith("2")) {  // 220 FRITZ!Box7490 FTP server ready.
    client.stop();
    return("Server hat eine nicht erwartete Antwort geliefert");
  }  
  send_string(client, String("USER ")+String(configdata.ftp_user));
  s=get_response(client); 
  if (!s.startsWith("3")) { // 331 Password required for wifiwaage.
    client.stop();
    return("Benutzer wurde nicht akzeptiert");
  }   
  send_string(client, String("PASS ") + String(configdata.ftp_pwd));
  s=get_response(client);  
  if (!s.startsWith("2")) { // 230 User wifiwaage logged in
    client.stop();
    return("Passwort wurde nicht akzeptiert");
  }     
  client.stop();
  return("ok");
}


bool log_data(String data) {  
  WiFiClient client;
  String s;
  bool ok=false;  
  if (!client.connect(configdata.ftp_host, configdata.ftp_port)) {      
    return(ok);
  }
  s=get_response(client);    
  if (!s.startsWith("2")) {  // 220 FRITZ!Box7490 FTP server ready.
    return(ok);
  }  
  send_string(client, String("USER ")+String(configdata.ftp_user));
  s=get_response(client); 
  if (!s.startsWith("3")) { // 331 Password required for wifiwaage.
    client.stop();
    return(ok);
  }   
  send_string(client, String("PASS ") + String(configdata.ftp_pwd));
  s=get_response(client);  
  if (!s.startsWith("2")) { // 230 User wifiwaage logged in
    client.stop();
    return(ok);
  }     
  send_string(client, "PASV");
  s=get_response(client);  
  if (!s.startsWith("227")) { // 227 Entering Passive Mode (192,168,173,1,145,49)
    client.stop();
    return(ok);
  }       
  int i1=s.indexOf("(");
  int i2=s.indexOf(")",i1);
  s = s.substring(i1+1,i2)+",";   // z.B. 192,168,173,1,166,134

  int ip[5], r=0, t=0;
  for (int i=0; i < s.length(); i++)
  { 
   if(s.charAt(i) == ',') 
    { 
      ip[t] = s.substring(r, i).toInt(); 
      r=(i+1); 
      t++; 
    }
  }  
  
  // siehe https://stackoverflow.com/questions/9624613/proper-user-of-stor-command
  // https://www.elektronik-kompendium.de/sites/net/0902241.htm
  unsigned int sendport = 256*ip[4] + ip[5]; // Port für passiven Zugang 
    
  // Eine Datei pro Tag
  time_t now = time(nullptr);
  struct tm *timeptr=localtime(&now);  
  char buff[32];
  strftime(buff,sizeof(buff),"%Y-%m-%d",timeptr);
  String filename="datalog_"+String(buff)+".csv"; 

    
  debug("Anfügen an Datei ");    
  debugln(String(configdata.ftp_path)+String("/")+filename);
    
  send_string(client, String("APPE ")+String(configdata.ftp_path)+String("/")+filename);

  // Zweite Verbindung zum Senden der Daten
  WiFiClient client_stor;
  if (!client_stor.connect(configdata.ftp_host, sendport)) {
    client.stop();
    return(ok);
  }
  send_string(client_stor, data);
  client_stor.stop();  
  s=get_response(client);
  if (!s.startsWith("150")) { // 150 Opening ASCII mode data connection for 'log/datalog_2021-12-08.csv'.
    client.stop();
    return(ok);
  }    
  ok=true;
  client.stop();
  return(ok);
}

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266WebServer.h>   // WebServer, siehe https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
#include <DNSServer.h>    // captive portal DNS, siehe https://github.com/esp8266/Arduino/blob/master/libraries/DNSServer/examples/CaptivePortal/CaptivePortal.ino
#include "time.h"
#include <EEPROM.h>
#include "HX711.h"

ADC_MODE(ADC_VCC);


// interner Versionszähler, wird geändert, wenn die neue Konfiguration (siehe eeromstruct) abweichend ist,
// zum Beispiel neue Felder hinzugefügt wurden. load_config (in config.ino) kann darauf reagieren und z.B.
// die neuen Felder mit einem Default belegen oder die gesamte Konfiguration migrieren.
const unsigned int wifiscale_version = 5;  
const char * release_version = "0.5.1";


HX711 scale;
// Anschluss des HX711
#define LOADCELL_DOUT_PIN 14  // GPIO14
#define LOADCELL_SCK_PIN 12   // GPIO12

float supply_power; // Spannungsmessung am ESP nach der Wiegung


// Anschluss des Buttons zum Start der Konfiguration
#define CONFIG_BUTTON 13 // GPIO13

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80
DNSServer dnsServer;            // DNS Server (für Captive Portal)


struct opmode_struct {
  const int CONFIG=0;
  const int RUN=1;
} OPERATION_MODE;



// Konfigruation, die im EEROM abgelegt wird
struct eeromstruct {
  char check[10];  // "WIFISCALE" + 0-Byte als Kontrolle, ob eine Konfiguration vorhanden ist.
  unsigned int version;   // interne Versionsnummer (siehe unten, release_version)
  char wifi_ssid[80];
  char wifi_pwd[80];  
  char ntp_server[80];
  char ftp_host[80];
  int ftp_port;
  char ftp_user[80];
  char ftp_pwd[80];
  char ftp_path[80];
  float scale_factor;
  float scale_tara;
  int sleeptime;  
  int heartbeat;
  float thresholdmin;
  float thresholdmax;
  int weightavgcount;
  int weightavgmaxtry;
  int minmaxthreshold;
  bool transmitincomplete;
  char release_version[16];  // Version-String
  
} configdata;



// Session des Webservers fuer die Konfiguration
struct configstruct {
  bool modified = false;
  bool wifi_ok = false;
  bool scale_ok = false;  
  bool ftp_ok = false; 
  bool all_ok=false;
  bool options_ok = true;
  float weight;
  float scale[3];
  float scalecalib[3];
  float scale_factor;
  float scale_tara;
  float last_test_weight;
} webconfig;



// Daten aus dem Wiegevorgang
struct hx711result {
  float weight; // Rohgewicht, noch nicht umgerechnet
  long weight_min;
  long weight_max;
  int status;
  int iterations; 
};



// RTC-Memory
struct rtcmem {
  char check[5];
  float lastweight; 
  unsigned long starttime;
  unsigned long timestamp;
  unsigned long lasttransfer; 
  long sleeptime;
  bool lastweightok;
} rtc;

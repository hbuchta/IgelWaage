# WiFi Waage für Igelfutterhaus

Inspiration: https://beelogger.de/sensoren/waegzellen_hx711/


Ähnliche Projekte im Internet

-   <https://www.instructables.com/Wi-Fi-Smart-Scale-with-ESP8266-Arduino-IDE-Adafrui/>

-   <https://how2electronics.com/iot-weighing-scale-hx711-load-cell-esp8266/>




# Vorbereitung

## Komponenten

-   Hardware

    -   ESP8266 möglichst minimal (ideal ESP-07S), am besten mit
        externer Antenne

    -   AD-Wandler für Waage: HX711

    -   Wägezellen

    -   Sonstiges

        -   Platte für den Aufbau der Wägezellen

        -   Elektronikbox

        -   Batteriehalter und Batterie

        -   Schalter

        -   Kabel und Anschlussbuchsen

-   Software

    -   Arduino IDE, [Arduino -
        Software](https://www.arduino.cc/en/Main/Software_)

    -   Serieller Treiber

        -   Für D1 Mini NodeMCU:
            <https://github.com/nodemcu/nodemcu-devkit/tree/master/Drivers>

        -   Siehe auch
            <https://tttapa.github.io/ESP8266/Chap03%20-%20Software.html>

    -   HX711 Treiber: <https://github.com/bogde/HX711>

### ESP 8266 07-S

Für den Batteriebetrieb bietet sich ein möglichst minimal bestückter
Controller (ESP8266) an. Ich habe diesen hier verwendet (Bezeichnung
ESP8266 **ESP-07S**):

[ZHITING ESP8266 Seriell zu WiFi-Funkmodul
ESP-07S](https://www.amazon.de/gp/product/B085B21DMB/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1)

![Ein Bild, das Text, Elektronik, Schaltkreis enthält. Automatisch
generierte
Beschreibung](./media/image1.png)

Um den Controller nutzen zu können, sollte dieser auf eine Platine
aufgelötet werden (SMD-Löten). Hier eignet sich diese Platine:

[AZDelivery 3 x Adapter Board Platte für ESP8266 ESP8266-12E / 12F,
ESP-07 und
ESP-08](https://www.amazon.de/gp/product/B071YQPLWH/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&th=1)

![](./media/image2.png)

Als Antenne habe ich diese genutzt:

[Bingfu RP-SMA WiFi Antenne 8 dBi 2,4/5/5,8 GHz, und 15cm IPEX IPX U.FL
auf RP-SMA-Buchse Kabel Kompatibel mit Intel Fenvi HP Dell Wireless
Mini-PCI Express PCIE-Netzwerkkarte WiFi-Adapter, 2
Stück](https://www.amazon.de/gp/product/B095KVPS34/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1)

![Ein Bild, das Text enthält. Automatisch generierte
Beschreibung](./media/image3.png)

### USB zu TTL Adapter

Für den Anschluss und die Programmierung ist ein USB zu TTL Adapter
erforderlich, z.B.

[AZDelivery 3 x CP2102 USB zu TTL Konverter HW-598 für 3,3V und
5V](https://www.amazon.de/gp/product/B07N2YLH26/ref=ppx_yo_dt_b_asin_title_o03_s00?ie=UTF8&psc=1)

![Ein Bild, das Text enthält. Automatisch generierte
Beschreibung](./media/image4.png)

Wichtig ist hier auf den Chipsatz zu achten, in diesem Fall ein CP2102.
Je nach Chipsatz sind unterschiedliche Treiber erforderlich (siehe
Abschnitt 1.2).

### AD Wandler HX711

Der AD-Wandler HX711 kann z.B. hier bestellt werden.

[Innovateking-EU 6 Stücke HX711 Wägezelle Gewicht Sensor Modul 24-Bit
Präzision AD Modul Dual-Kanal für Arduino
Waage](https://www.amazon.de/gp/product/B07TWLP3X8/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1)

![Ein Bild, das Text, Elektronik, Schaltkreis enthält. Automatisch
generierte
Beschreibung](./media/image5.png)

### Ladezellen

Als Waage habe ich diese hier bestellt:

[YOUMILE 4er Pack DIY 110lbs 50KG Halbbrücken Wägezelle Gewicht
Dehnungssensor Widerstand + HX711 AD
Modul](https://www.amazon.de/gp/product/B07TWLP3X8/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1)

Hier ist auch direkt ein HX711 Modul enthalten, allerdings war mir hier
die Platine zu groß. Die Platine aus Abschnitt 1.1.3 ist deutlich
kleiner.

![](./media/image6.png)

### Batterie und Batteriehalter

Als Batterie kommt eine Batterie **LiFePo** 18650 (3,2 V) zum Einsatz
(Achtung: kein Lithium Ionen Akku keine NiMH-Akkus!), z.B.

[4 Stück HAKADI 3.2V LiFePo4 Akku 1500mAh Mit 3000+ Zyklen Lebensdauer
wiederaufladbare Lithium Eisenphosphat Batterien 3C-5C Entladung für
Taschenlampen,
Elektroroller](https://www.amazon.de/gp/product/B09J4N36WT/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&th=1)

![](./media/image7.png)

Ein entsprechendes Batteriefach wird auch benötigt, z.B.

[2 Stück 18650 Einzelbatteriehalter, 1 Steckplatz, schwarzer
Verriegelungsfederbatteriehalter](https://www.amazon.de/gp/product/B08K32JPZW/ref=ppx_yo_dt_b_asin_title_o05_s00?ie=UTF8&psc=1)

![Ein Bild, das Waffe enthält. Automatisch generierte
Beschreibung](./media/image8.png)

Alternativ kann ein Halter für 2 Batterien genutzt werden. Die Batterien
müssen dann parallel (nicht in Reihe!!!) geschaltet werden, z.B.:

[sourcing map Batteriefach Aufbewahrungsbox 2 Steckplätze x 3,7 V
Batteriehalter für 2 x 18650 Batterie 2
Stück](https://www.amazon.de/gp/product/B095KVPS34/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1)

![](./media/image9.png)

Hier müssen beide Batterien in der gleichen Orientierung eingelegt und
die Anschlüsse jeweils auf Kopf- und Fußseite verbunden werden. Achtung:
Im Fach ist selbst eine Polung angegeben (gedreht). Diese kann man auch
nutzen, dann müssen die Anschlüsse aber über Kreuz abgeleitet werden
(beide Pluspole zusammen, beide Minuspole zusammen). Am Ende darf auch
bei zwei Batterien die Spannung nur 3,2V betragen (direkt nach dem
Aufladen kann sie etwas höher sein, aber bei 6V liegen die Zellen in
Reihe, was den ESP zerstören würde).

### Wasserdichte Elektronikdose

Ich habe diese Dose verwendet:

[Kunststoffgehäuse IP66 ABS Plastik Elektronische Gehäuse Box
Anschlußdose Wasserdicht Schaltschränke Stromverteilung Zählerkasten
Leergehäuse Kunststoff
(100x68x50mm)](https://www.amazon.de/gp/product/B07LF6TMLD/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&th=1)

![Ein Bild, das Wand, drinnen, Gefäß enthält. Automatisch generierte
Beschreibung](./media/image10.png)

### Sonstiges

Für den Schalter habe ich dieses Set genutzt:

[APIELE 5 Stücke Wasserdichter runde Kippschalter,2 Position EIN-Aus mit
LED-Schalter SPST 3 Pins Wippschalter,12V 20A Taste Rocker Kippschalter
Für Auto, Boot, Haushaltsgeräte KCD1-8-101NW
(grün)](https://www.amazon.de/gp/product/B08LKSYSRP/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&th=1)

### Platine

Die Platine kann mit einem PCB Tool wie Fritzing erstellt werden:

![](./media/image11.png)

Bei Aisler.net sieht die Platine so aus (Oberseite):

![](./media/image12.png)

Hinweis: Das schöne Bild mit dem Igel ist urheberrechtlich geschützt. Es kann [hier bei Etsy](https://www.etsy.com/de/listing/661210954/igel-einzigartige-tier-datei-svg-ai-dxf) gekauft werden. In der Platine, die hier im Ordner abgelegt ist, ist das Bild nicht enthalten.

Die fertig bestückte Platine:

![](./media/image13.png)

Und so sieht das dann in der Elektronikbox aus.

![](./media/image14.png)

## Seriellen Treiber installieren

Der Serielle Treiber richtet sich nach dem USB zu TTL Adapter bzw. bei
Boards bei denen dieser integriert ist (USB-Anschluss auf dem Board,
z.B. NodeMCU) nach dem Board.

### CP2102 USB zu TTL Adapter oder Wemos D1 Mini

Der Treiber kann hier heruntergeladen werden:\
<https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers>

Treiber ist CP210x Windows Driver, nicht CP210x Universal Windows Driver

![Ein Bild, das Text enthält. Automatisch generierte
Beschreibung](./media/image15.png)

Nach dem Download die ZIP-Datei entpacken und die INF-Datei installieren
(Kontextmenü):

![Ein Bild, das Text enthält. Automatisch generierte
Beschreibung](./media/image16.png)

Den genutzten Port (virtueller COM-Anschluss) sieht man im
Gerätemanager. Hier z.B. COM4. Dieser Port muss dann in der Arduino IDE
ausgewählt werden.

![Ein Bild, das Text enthält. Automatisch generierte
Beschreibung](./media/image17.png)

### D1 Wemos NodeMCU (aufgedruckte WLAN Antenne)

Der Treiber kann hier heruntergeladen werden:

<https://tttapa.github.io/ESP8266/Chap03%20-%20Software.html>

![](./media/image18.png)

Der Download-Link für die NodeMCU Boards verweist auf:

<https://github.com/nodemcu/nodemcu-devkit/tree/master/Drivers>

Hier die Windows-Version herunterladen, entpacken und starten.

**Der Microcontroller dafür bereits vor der Installation per USB
angeschließen!**

![Ein Bild, das Text enthält. Automatisch generierte
Beschreibung](./media/image19.png)

![Ein Bild, das Text enthält. Automatisch generierte
Beschreibung](./media/image20.png)

**Wenn das Board bei der Installation nicht über das serielle Kabel
angeschlossen war (oder das Kabel defekt ist bzw. kein Datenkabel war)
kommt die Meldung, dass der Treiber vorläufig installiert wurde.**

**Also: Board vor der Installation anschließen**

Den genutzten Port (virtuelle COM-Schnittstellen), sieht man im
Gerätemanager. Hier ist es COM3.

![](./media/image21.png)

Dieser Port muss in der Arduino IDE eingestellt werden.

## Arduino IDE einrichten

Arduino IDE installieren.

ESP8266 Core Version 2.0 für Arduino installieren.

Siehe z.B.

-   <https://circuitjournal.com/esp8266-with-arduino-ide>

-   [GitHub - esp8266/Arduino: ESP8266 core for
    Arduino](https://github.com/esp8266/Arduino)

Hier findet sich auch die Dokumentation des Arduino Cores:

[GitHub - esp8266/Arduino: ESP8266 core for
Arduino](https://github.com/esp8266/Arduino)

Unter Datei/Voreinstellungen die URL in das Feld "Zusätzliche
Boardverwalter-URLs" eintragen und dann auf ok klicken.

https://arduino.esp8266.com/stable/package_esp8266com_index.json

![](./media/image22.png)

Dann unter Werkzeuge/Boardverwalter über das Suchfeld "esp8266" die
"eps8266" Boards hinzufügen:

![](./media/image23.png)

Anschließend das Board auswählen unter Werkzeuge/Board: "Name"/ESP8266
Boards.

Ja nach verwendetem Board kommen unterschiedliche Einstellungen zur
Anwendung.

  -------------------------------------------------------------------------------------------------------------------------
  ESP-07S                                                           NodeMCU
  ----------------------------------------------------------------- -------------------------------------------------------
  ![Ein Bild, das Text enthält. Automatisch generierte              ![](./media/image25.png)
  Beschreibung](./media/image24.png)  

  -------------------------------------------------------------------------------------------------------------------------

Diese Einstellungen erreicht man über das Werkzeuge-Menü:

![](./media/image26.png)

Im Menü "Werkzeuge" können weitere Einstellungen für das Board
vorgenommen werden. Wichtig ist die Port-Einstellung (gemäß dem Port,
den der serielle USB Treiber nutzt).

Ist „Port" ausgegraut (wie hier im Screenshot) funktioniert der Treiber
noch nicht (siehe Abschnitt 1.2) oder es ist kein Gerät angeschlossen.

Ein möglicher Fehler ist auch, dass das Micro-USB Kabel nicht geeignet
ist (es muss ein Datenkabel sein und nicht nur die Leitungen zum Laden
haben).

Mit installiertem Treiber kann auch der Port gewählt in der Arduino IDE
gewählt werden:

![](./media/image27.png)

Der Port muss durch Auswahl einmal gewählt werden. Solange dort „Port"
steht, ist noch kein Port gesetzt. Danach sollte das so aussehen:

![Ein Bild, das Text enthält. Automatisch generierte
Beschreibung](./media/image28.png)

Der serielle Monitor findet sich unter Werkzeuge/Serieller Monitor. Hier
muss die richtige Baud-Zahl eingetragen (identisch mit dem
Serial.begin-Wert im Code), bei mir 115200.

![](./media/image29.png)

## Bibliothek HX711 installieren

Die HX711 Bibliothek, die ich genutzt haben, findet sich hier:

<https://github.com/bogde/HX711>

Sie kann einfach über die Bibliotheksverwaltung der IDE installiert
werden.

![](./media/image30.png)

# Hardware zusammensetzen

## Waage aufbauen / HX711 anschließen

Die vier Pads müssen nach diesem Muster verschaltet werden:

![](./media/image31.png)

# Fritz!Box als Zeit- FTP Server

## Zeitserver

Im Menü Heimnetz, den Punkt Netzwerk auswählen, dann Reiter
Netzwerkeinstellungen und ganz nach unten scrollen. Den Bereich „weitere
Einstellungen" aufklappen. Dort kann das Häkchen gesetzt werden bei
„FRITZ!Box als Zeitserver im Heimnetz bereitstellen".

![](./media/image32.png)

## FTP

Zuerst ein Verzeichnis anlegen. Am einfachsten geht das mit dem
FRITZ!NAS Client:

![](./media/image33.png)

Ich habe ein Verzeichnis wifiwaage auf dem USB Stick angelegt. Darunter
am besten direkt ein Verzeichnis log, in das die Wiegeprotokolle
geschrieben werden.

Dann in der FRITZ!Box Oberfläche auf System, FRITZ!Box-Benutzer klicken
und einen neuen Benutzer anlegen.

**Hinweis**: Es empfiehlt sich zwei getrennte Benutzer einzurichten,
einen für die Waage (Übertragung der Wiegedaten) und einen zum Auslesen.
Ansonsten hatte ich das Problem, dass es Schreibfehler gibt, wenn die
Waage übertragen will und gleichzeitig der Benutzer zum Lesen eingeloggt
ist.

Name: wifiwaage

Passwort: bella123

Bei Berechtigungen das Häkchen bei „Zugang zu NAS-Inhalten" setzen.

![](./media/image34.png)

In dem dann aufklappenden Bereich auf „Verzeichnis hinzufügen" klicken
und das gerade angelegte Verzeichnis auswählen (nicht das
Log-Unterverzeichnis):

![Ein Bild, das Text enthält. Automatisch generierte
Beschreibung](./media/image35.png)

Mit einem Client wie WinSCP (auch FileZilla, FireFTP oder viele andere)
kann man den Zugang jetzt püfen und später auch Dateien herunterladen.
Wichtig ist, die Verbindung als FTP (nicht SFTP) zu konfigurieren:

![](./media/image36.png)

Ein Standardverzeichnis muss nicht gewählt werden (man befindet sich
automatisch in dem freigegeben Ordner). Das Passwort kann hier mit
gespeichert werden.

![Ein Bild, das Text enthält. Automatisch generierte
Beschreibung](./media/image37.png)

![Ein Bild, das Text enthält. Automatisch generierte
Beschreibung](./media/image38.png)

# Konfiguration der Waage

## Waage im Konfigurationsmodus starten und verbinden

Damit die fertig gebaute Waage auch funktioniert, sind noch einige Konfigurationseinstellungen erforderlich.
Das wird die Waage im Konfigurationsmodus gestartet:

1. Wenn die Waage ausgeschaltet (vom Strom getrennt) ist, so wird der Taster Config gedrückt und gedrückt gehalten. Die Waage wird eingeschaltet. Nach etwa 4 Sekunden kann der Taster Config losgelassen werden.
2. Wenn die Waage bereits läuft (mit Strom verbunden), so wird der Taster Config gedrückt und gedrückt gehalten. Anschließend wird der Taster Reset für eine Sekunde gedrückt. Nach etwa 4 Sekunden kann der Taster Config nun losgelassen werden.

Die Waage sollte sich anschließend im Konfigurationszustand befinden. In diesem Zustand führt die Waage keine Wiegungen aus, sondern startet einen WLAN-Hotspot, mit dem man sich nun verbinden muss.

Der Hotspot sollte unter dem Namen WifiScale-xxxxx (xxxxx=Chip-ID des verbauten ESP 8266) sichtbar sein. Das Passtwort lautet "hedgehog" (ohne die Anführungszeichen).

Mit einem Rechner, Tablet oder Smartphone verbindet man sich nun mit diesem WLAN. Achtung: Das betreffende Gerät verfügt über einen keinen Internetzugang, solange die Verbindung mit der Waage hergestellt ist.

Auf dem Gerät öffnet man nun einen Browser und in dem Browser die Webseite `http://192.168.1.4`. Es kann auch eine andere Adresse angegeben werden. Eigentlich sollte eine beliebe Webseite zur Konfiguration führen, aber die IP-Adresse funkioniert am sichersten.

Die Waage zeigt dann ihr Hauptmenü an:

<img src="./media/config1.PNG"  width="500">

Beim ersten Start wird die Waage anstellt der grünen OKs rote TODOs anzeigen.
Man kann nun von oben nach unten die Punkte durchgehen und die betreffenden Einstellungen durch Klicken auf `ändern` vornehmen.


## WiFi Konfiguration

In ersten Schritt wird die WLAN-Konfiguration hergestellt. Erforderlich ist die SSIS des WLAN-Routers und das Passwort, sowie ein Zeitserver. Dieser kann auf dem WLAN-Router selbst bereitgestellt werden (siehe oben) oder aus dem Internet bezogen werden (Adresse z.B. 0.de.pool.ntp.org). Entsprechende Server lassen sich über eine Suchmaschine finden. Bei mir ist die Adresse fritz.box, da der Zeitserver über die Fritzbox selbst bereitgestellt wird.

<img src="./media/config2.PNG"  width="340">

## FTP-Server Konfiguration

Damit die Waage die Messdaten im Netzwerk speichern kann, ist ein Zugriff auf einen FTP-Server erforderlich. Dieser muss den passiven Modus unterstützen. Die entsprechenden Daten des FTP-Servers (Server, Port, User, Passwort) können in der folgenden Maske eingestellt werden. Außerdem wird ein Verzeichnis angegeben, in das die Waage schreiben soll.

<img src="./media/config3.PNG"  width="340">

## Kalibrierung der Waage

Um die Waage zu kalibrieren, sind 2 bekannte Gewichte erforderlich. Außerdem benötigt man eine Platte, um die Gewichte auf die Waage stellen zu können. Die Waage führt durch die Konfiguration:

<img src="./media/config5.PNG"  width="340">

Die erste Messung erfolgt ohne Gewicht (auch ohne die Platte) und gibt die absolute Nullstellung der Wiegezellen an. Wenn die Waage frei von Gewichten ist, drückt man auf Weiter und die Messung wird durchgeführt.
Anschließend wird die Platte aufgelegt und ein Gewicht auf die Platte gelegt. In das Eingabefeld wird nun das Gesamtgewicht (Platte und aufgelegtes Gewicht) eingegeben.

<img src="./media/config6.PNG"  width="340">

Anschließend drückt man auf Weiter. Dieser Vorgang wird noch mal mit einem weiteren Gewicht wiederholt.
Im letzten Schritt zeigt die Waage das Ergebnis der Kalibrierung an.
Wenn keine Unstimmigkeiten erkannt werden, kann das Ergebnis übernommen werden. Andernfalls muss die Kalibrierung wiederholt werden.

## Optionen

Die letzten Einstellungen beziehen sich auf die Optionen. Jede einzelne Option ist in der Spalte Beschreibung erläutert. Die Voreinstellungen können ein erster Anhalt sein.

<img src="./media/config4.PNG"  width="340">


# Protokolldatei

Die Ergebnisse werden in Tagesdateien geschrieben. Während des Tages hängt die Waage neue Wiegungen an die Datei an. Die Dateien haben das Muster datalog-jjjj-mm-tt.csv.

Beispiel einer Protokolldatei:
```
2022-10-12 00:34:35,866,9665,31373,72,0,-70,3.44
2022-10-12 00:49:26,-842,8814,11795,15,0,-71,3.44
2022-10-12 02:50:26, 0,8813,11774,15,0,-68,3.44
2022-10-12 03:41:14,870,9676,31638,21,0,-71,3.44
2022-10-12 03:51:40,-845,8814,11801,15,0,-72,3.44
2022-10-12 05:52:42, 0,8841,12422,15,0,-71,3.44
2022-10-12 07:53:42, 0,8797,11421,15,0,-73,3.44
2022-10-12 09:54:45, 0,8805,11598,15,0,-76,3.44
2022-10-12 11:55:54, 0,8911,14045,15,0,-75,3.45
2022-10-12 13:16:50,36,9097,18317,72,0,-76,3.46
2022-10-12 15:18:01, 0,9215,21021,15,0,-75,3.46
2022-10-12 17:19:06, 0,9087,18088,15,0,-75,3.46
2022-10-12 18:56:12,840,9717,32585,15,0,-76,3.46
2022-10-12 19:08:09,-829,8863,12926,15,0,-75,3.46
2022-10-12 19:42:30,809,9630,30568,15,0,-77,3.46
2022-10-12 19:47:02,-817,8807,11639,55,0,-76,3.46
2022-10-12 21:18:08,833,9614,30212,21,0,-74,3.46
2022-10-12 21:27:14,-834,8766,10704,98,0,-74,3.45
2022-10-12 21:58:30,829,9590,29659,38,0,-73,3.45
2022-10-12 22:04:26,-839,8758,10515,15,0,-73,3.46
2022-10-12 23:23:44,849,9606,30026,70,0,-72,3.45
2022-10-12 23:31:08,-845,8756,10463,29,0,-73,3.45
```

Die Protokolldatei enthält diese Informationen

Spalte | Beschreibung | Beispiel
------ | ------------ | --------
1 | Datum und Uhrzeit des Eintrags | 2022-10-12 00:34:35
2 | Gewichtsänderung in Gramm | 866
3 | Absolutes Gewicht auf der Waage in Gramm | 9665
4 | Rohmessung der Waage | 31373
5 | Anzahl erforderlicher Messungen bis Wiegung stabil | 72
6 | Status (0=ok, 1=nicht ok) | 0
7 | WLAN Signal | -70
8 | Spannung in Volt | 3.44
# CitRad Sensoreinheit

# Installation einer Hex-Datei aus den Releases

1. Hex-Datei vom aktuellen Release [herunterladen](https://github.com/fablabcb/CitRad-SensorUnit/releases).
2. Unter Linux, **teensy-loader-cli** aus den Paketquellen installieren (gängige Distributionen stellen das Paket über den Paketmanager bereit). Ansonsten von der [Website des Herstellers](https://www.pjrc.com/teensy/loader.html) installieren und den Anweisungen dort folgen.
3. Teensy via USB mit dem Computer verbinden und SD-Karte einsetzen.
4. Auf dem Terminal Folgendes ausführen: `teensy_loader_cli -v --mcu=teensy40 -w <Pfad zu der Hex-Datei>` und den Anweisungen auf dem Bildschirm folgen (den Knopf auf dem Teensy drücken). Wenn das Program einen Fehler meldet (**Programming...error writing to Teensy**), den Befehl einfach nochmal ausführen. Wenn das Kommando mit **Booting** endet, hat alles geklappt.
5. Die Uhrzeit auf dem Teensy einstellen, indem Folgendes Kommando ebenfalls im Terminal ausgeführt wird: `date -u +T%s > /dev/ttyACM0`.

## Hinweis

Auf der SD-Karte sollten jetzt Dateien mit alten und mit neuen Zeitstempeln liegen (Zeit ist im Dateinamen zu finden). Diese können einfach gelöscht werden und sollten nicht in ein Portal hochgeladen werden.

## Alternative zum Setzen der Zeit

Wenn kein Linux-System zur Verfügung steht und die Software vom Hersteller die Zeit nicht setzt, besteht eine Alternative darin, die Zeit über eine App oder ein anderes Programm zu setzen. Im Prinzip muss nur eine Zeichenkette der Art "T1720883807" an den Teensy geschickt werden. Möglich ist das z.B. über [diese App](https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal&hl=de) oder die Arduino IDE.

Die Zeichenkette setzt sich aus dem Buchstaben "T" und der aktuellen UNIX-Zeit in UTC zusammen. Diese bekommt man von [hier](https://www.unixtimestamp.com/de). Einfach die große Zahl oben rechts kopieren und ein "T" davor schreiben. Die Zeit muss nicht exakt sein und man kann einfach ein paar Sekunden draufrechnen.

### Arduino IDE

- installieren
- Serial Monitor öffnen und mit dem Teensy verbinden
- aktuelle Zeit als Befehl senden, z.B. "T1720883807"

### App "USB Serial Terminal"

Zuerst muss sicher gestellt werden, dass man den Sensor auch mit dem Handy verbinden kann! Für die Verbindung von (wahrscheinlich) USB-C zu Micro-USB am Sensor benötigt man einen entsprechenden USB-Adapter.

Sind Handy und Sensor verbunden, öffnet man die App, geht im Menü auf "USB Devices" und wählt den ersten Eintrag aus (z.B. "Serial device - CDC - Port 1"). In der folgenden Ansicht sollten dann direkt schon Nachrichten vom Teensy auftauchen. Jetzt gibt man unten den passenden Zeit-Befehl ein und drückt auf senden. Der Teensy antwortet mit der eingestellten Zeit ("Time set to: ...").

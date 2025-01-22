# CitRad 3D-Druck und Lasercut Teile

Die detaillierte Bauanleitung findet sich unter [citrad.de/docs/Bauanleitung](https://citrad.de/docs/bauanleitung/). 

Hier finden sich die Lasercut- und 3D-Druck Teile:

- Mit dem Lasercutter die Einlegeplatte 'CasingInlay.svg' aus 3mm Acryl oder ähnlichem schneiden. Bei mehreren `CasingInlay_6x.svg` verwenden.
- Die Mstbefestigung `MastFastening.svg` und die Abstandhalter `MastFasteningWasher.svg` aus 5mm Acryl oder ähnlichem schneiden.
- `SensorRadarMount.scad` in [OpenSCAD](https://openscad.org/) öffnen, als STL exportieren und dann 3D-drucken

# Migrated from Sensor Unit

## Hardware

This prototype works with the [IPS-354](https://media.digikey.com/pdf/Data%20Sheets/InnoSenT/200730_Data%20Sheet_IPS-354_V1.5.pdf)
Radar Sensor from InnoSenT connected directly to the mic input of the [Teensy Audio Board](https://www.pjrc.com/store/teensy3_audio.html). 

## Wiring

| IPS-354           | Audio Board | Teensy |
| ---------------- | ----------- | ------ |
| Pin 2 (enable)   | -           | Pin 22 |
| Pin 3 (Vcc)      | -           | Vcc    |
| Pin 4 (Gnd)      | Mic Gnd     | Gnd    |
| Pin 5 (signal I) | Mic         | -      |

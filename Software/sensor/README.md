# CitRad Sensor Unit

Kurzanleitung auf [Deutsch](docs/README_de.md).

The sensor is comprised of a Teensy, a matching Audio Board and a Radar Sensor ([see the Hardware README for more details](../../Hardware/README.md)). Together they can detect cars, pedestrians and bicycles from the roadside or even the third floor of a building.

The Teensy continuously calculates a 1024 point FFT, analyzes the signal and stores result data on the SD card. It can also transmit the raw binary data over USB so that it can be monitored by the [Companion App](../mobileCompanion/README.md).

# Install a Hex File from the Release Section

1. Download the sensor hex file of the current release (see the release section of this repository).
2. Connect your Teensy via USB and insert the SD card.
3. Download the manufacturer's software for Teensy here https://www.pjrc.com/teensy/loader.html and follow the instructions there.

# Build and Run

## Requirements

We suggest using Linux, make and arduino-cli as this is the most tested and streamlined environment. The steps below also focus on that environment.

* arduino-cli (or Arduino IDE 2.0)
* Audio Library (see below)
* Make (optional if using Arduino IDE)
* teensy-loader-cli (may be optional if using Arduino IDE)

### Installation of required Software

Install **make** using your package manager. Then (on the terminal in this directory where the Makefile is) run `make testSetup` and follow the instructions until it is happy.

### But I want to use the IDE

These steps might be incomplete.

* In the (board manager) settings, add https://www.pjrc.com/Teensy/package_Teensy_index.json as additional URL and then install the Teensy (4.0) board.
* It is important to set the Arduino IDE to dual serial: "Tools > USB Type: Dual Serial". More infos under https://github.com/TeensyUser/doc/wiki/Serial and https://www.pjrc.com/Teensy/td_serial.html

### 32bit Audio Library

To get better input resulution we use a 32bit audio library that was forked from the original Teensy Audio library: [OpenAudio_ArduinoLibrary](https://github.com/chipaudette/OpenAudio_ArduinoLibrary). Install it according to the instructions given on the repository page.

Similar to the original audio library it has a Design tool (not required): [OpenAudio Design Tool](http://www.janbob.com/electron/OpenAudio_Design_Tool/index.html)

## Build

Run `make` and wait for it to complete. The first build will take some time since Arduino does some weird stuff. Subsequent builds will be faster as long as you do not touch the sensor.ino file.

When using Arduino IDE: load sensor.ino and compile/run that.

## Flash & Run

1. Put the SD card into the sensor.
2. Connect the Teensy via USB to your computer.
3. Run `make deploy` (you can also run `make deployNoBuild` if you already build and did not make any changes to the code). If you get an error while programming the Teensy, simply try to run the command again.
4. Press the button on the Teensy as the instructions on the screen tell you.

The sensor will automatically restart and begin recording data.

## Real Time Clock (RTC)

Make sure the coin cell is on (Off->On), connect the running Teensy to your computer and run:

```
date -u +T%s > /dev/ttyACM0
```

The command sends the letter "T" followed by the current Linux time (in UTC) to the Teensy. Replace `ttyACM0` with your serial device name (should not be required in most cases).

The Real Time Clock needs to be set again after changing the coin cell (do that when you recognize the files on the SD cards have outdated timestamps).

# Getting data from the sensor

There are 2 ways to get data from the sensor. Note that even when using the Serial interface, an SD card needs to be connected since the sensor currently only starts working with a card inserted.

## SD Card

The program can write 3 different types of files to the SD card:
* raw binary FFT data (relatively big)
* metrics as CSV files (intermediate data; much smaller)
* detections as CSV files (only information about detected cars; smallest)

The raw file and metrics are mainly used to develop the detection algorithm or for debugging. The detections file contains what we are most interested in.

Please see the file format [here](docs/FileFormats.md).

## Serial / USB

The Teensy opens two serial connections over USB. The first (port 0) is a text interface primarily used for debugging and can be read by any software capable of monitoring serial data (the Arduino IDE can do that too, of course).

```
# this will show your connected boards
arduino-cli board list
# to view the configured board; see Makefile - you will likely need to change the --port option
make monitor
# or type it by hand; adjust --port:
arduino-cli monitor --fqbn teensy:avr:teensy40 --port usb3/3-4/3-4.2 --protocol teensy
```

The second connection (port 1) is used for raw binary data transmission. To view that data please refer to the [Companion App](../mobileCompanion/README.md)

# Additional Information

## Sensor Configuration File on the SD Card

The configuration file is optional. If you have no need of changing anything, please don't use it.

If you have multiple sensors or you want to tinker, open the SD card (on any device) and create a file called `config.txt` in the root directory. Paste the following into that file and change the string `MySensorA` to something you like. All files created on that SD card will have this string prefixed to the file names.

```
filePrefix: MySensorA_
writeDataToSdCard: true
writeRawData: false
maxSecondsPerFile: 3600
```

All other values above are set to the default values. See Config.cpp for all supported options.

**Note: Please do not upload data with custom configurations to offical sensor data portals.**

## Teensy Audio Board Prototype

The IPS-354 has a builtin gain of 27dB. The mic input of the audio board can regulate it's gain from 0 to 63dB. When measured directly at the edge of the street the signal goes into clipping, so that the mic gain has to be set to about 55 to 58 dB.

## IQ FFT

The 32bit audio library supports complex FFT calculation with I and Q channel. The [IPS-354](https://media.digikey.com/pdf/Data%20Sheets/InnoSenT/200730_Data%20Sheet_IPS-354_V1.5.pdf) sends 
I and Q signals from which the direction of the radar signal can be derived. To record I and Q signal we have to use the linein input instead of the mic input. This has only 0-22dB gain 
instead of up to 63dB gain on the mic input. But it offers two channels that we need for the IQ signal. The input sensitivity seems enough for the IPS-354.

We have done a correction for I-Q imbalance after [this instruction](https://www.faculty.ece.vt.edu/swe/argus/iqbal.pdf).

We develop this type of data analysis in the [IQ-fft branch](https://github.com/fablabcb/CityRadar/tree/IQ-fft/Teensy_prototype). 

The wiring is as follows:

| IPS-354          | Audio Board | Teensy |
| ---------------- | ----------- | ------ |
| Pin 2 (enable)   | -           | Pin 22 |
| Pin 3 (Vcc)      | -           | Vcc    |
| Pin 4 (Gnd)      | Linein Gnd L| Gnd    |
| Pin 5 (signal I) | Linein L    | -      |
| Pin 6 (signal Q) | Linein R    | -      |

### SD noise problems

At the moment writing to SD creates noise in the data. Some ideas how to tackle this:

* Some ideas from here: [How-to-avoid-noise-when-writing-to-the-microSD](https://forum.pjrc.com/threads/25326-How-to-avoid-noise-when-writing-to-the-microSD)
* change main clock speed -> didn't improve noise
* change slew rate of the pins: https://www.pjrc.com/Teensy/IMXRT1060RM_rev2.pdf#page=695 with `IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_00 = 0b0000'0000'0000'0000'0001'0000'1011'0000;`
* add condensators to buffer high Vcc demands
  * 10kOhm to buffer sensor input voltage didn't help
* use SD card of Teensy 4.1 instead of the audioshield. This itself doesn't improve. But maybe the SD pins to the audioboard should not be connected?
* pause data aquisition while writing to SD

### RTC

The time used internally is in UTC!

We use the internal RTC of the Teensy to add correct timestamps to the stored data. A coin cell battery needs to be connected to keep the internal RTC running. More on this here: [Teensy Time library explanation and examples](https://www.pjrc.com/Teensy/td_libs_Time.html).

The TimeSerial example under `File > Examples > Time > TimeSerial` shows how to set the RTC time via serial input.
Here is a forum Thread on [how to access the internal RTC in a Teensy 4.0](https://forum.pjrc.com/threads/60317-How-to-access-the-internal-RTC-in-a-Teensy-4-0).

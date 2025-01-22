# CitRad Companion App

The Companion App can be used to monitor the sensor or view binary files. It is not considered to be ready for release
and needs some polish - as does this documentation. You do not need this software if you only want to use the sensor.

The app can process the sensor's binary data and then does the same signal processing (also uses the same code to do so) and visualizes the results. This can be useful to test if the sensor is actually working and if the object detection is functional.

## Requirements

List might be incomplete:

### x86

* Qt5 or Qt6 (note that for android you need to use Qt5 as of now)
  * Core (base)
  * Quick (quickcontrols2)
  * Qml (declarative)
  * SerialPort (serialport)
  * and their *-devel counterparts.
* gcc
* cmake
* libasan (might come with gcc)
* make
* boost (for tests; tests can be disabled in CMakeLists.txt)

### Android

Setting up Android is a tricky thing. Be sure to familarize yourself with the overall requirements to run Qt apps on android. Then, make sure you have the correct Qt version for the Android version you are targeting. If you manage to get this to run on Qt6, please get in contact.

* all of the above except the Qt libraries
* Android SDK (Makefile -> ANDROID_SDK_DIR)
* Android NDK (Makefile -> ANDROID_NDK_DIR)
* Qt5 Libraries compiled for Android (Makefile -> ANDROID_QT_DIR)
* openjdk-8 (also see Makefile for notes on Java)
* https://github.com/LaurentGomila/qt-android-cmake.git (clone into a folder "external/"; see CMakeLists.txt)

## Build and Run

### x86

Simply `make && make run`.

### Android

1. `make android`
2. Connect your phone, enable developer mode and set correct USB mode.
3. `make deploy`

# Usage

There are 3 tabs. The first can be used to view live data (connect to port 1) or open a file. When using a file, use the buttons to load the next samples. The second tab shows debug data from the sensor (connect to port 0). The third tab simply lists all detections.

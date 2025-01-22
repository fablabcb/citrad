# Software Components

## Sensor Unit

The "Sensor Unit" is the radar hardware required to collect data. You can find the software running on that hardware here. You can also find compiled binaries in the release section. Please consult the readme for instructions on how to flash the firmware onto the sensor.

[Jump to Sensor Readme](sensor/README.md)


## Sensor Companion App

The companion app can be used to monitor and analyze sensor unit data live in the field or analyze binary files that have been recorded previously.

[Jump to Companion Readme](mobileCompanion/README.md)

## Data Processing written in R

This contains the original analyzer software written in R. It is not required to run the sensor or the companion app.

[Jump to R-Method Readme](<data processing method/README.md>)

# General Processing

The sensor data is treated as Audio data, processed using an FFT and then visualized as a [waterfall diagram](https://en.wikipedia.org/wiki/Waterfall_plot). The Teensy calculates additional data like the frequency with the highest amplitude, which is then drawn as a line into the graph. This represents the mean speed of a moving object detected by the radar. 

The conversion from FFT bin to frequency is:
`fft_bin_width = sample_rate/1024 = 44100 Hz/1024 = 43.0 Hz`

The conversion from frequency to speed (at optimal angle, meaning the object comes directly onto the sensor) is:
`speed = frequency/44`

Here a measurement done from 2m distance from the street edge using an early prototype: 

![](FFT_visualisation/screenshots/radar_spectrum_2m_from_street_edge_cars_pedestrians_annotated.png)

Here a measurement with a pedestrian up close where arm and leg movement can be seen in the signal. Cars are on the opposite side of the street and hence with fainter signal:

![pedestrian close](./FFT_visualisation/screenshots/radar_spectrum_2m_from_street_edge_cars_opposite_pedestrian_close.png)

Here a measurement closer to the street with cars traveling both directions:	

![cars both lanes](FFT_visualisation/screenshots/radar_spectrum_1m_form_street_edge_cars_both_lanes.png)

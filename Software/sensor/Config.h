#ifndef CONFIG_H
#define CONFIG_H

#include "AudioSystem.h"

struct Config
{
    AudioSystem::Config audio;

    const float max_pedestrian_speed = 10.0; // m/s; speed under which signals are detected as pedestrians
    const float send_max_speed = 500;        // don't send (and store) spectral data higher than this speed
    const float TRIGGER_AMPLITUDE = 100;     // trigger threshold for mean amplitude to signify a car passing by
    const long COOL_DOWN_PERIOD = 1000;      // cool down period for trigger signal in milliseconds

    const bool writeDataToSdCard = true; // write data to SD card?
    const bool write8bit = true;         // write data as 8bit binary (to save disk space)
    const bool writeRawData = true;      // write raw spectral data to SD?
    const bool writeCsvData = true;      // write calculated metrix to csv table?
};

#endif
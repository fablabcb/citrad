#include "AudioSystem.h"
#include "Config.hpp"
#include "FileIO.hpp"
#include "SerialIO.hpp"
#include "SignalAnalyzer.hpp"
#include "functions.h"

#include <SerialFlash.h>
#include <TimeLib.h> // year/month/etc
#include <Wire.h>
#include <utility/imxrt_hw.h>

AudioSystem audio;
SignalAnalyzer signalAnalyzer;
SignalAnalyzer::Results results; // global to optimize for speed
Config config;

FileIO fileWriter;
SerialIO serialIO;

bool canWriteData = false;

void onSdCardActive()
{
    canWriteData = true;
    Serial.println("(I) SD card initialized");
    auto configFromFile = fileWriter.readConfigFile();
    config.process(configFromFile);
}

void setup()
{
    setSyncProvider(getTeensy3Time);
    setI2SFreq(config.audio.sampleRate);

    // TODO maybe move to audio
    pinMode(PIN_A3, OUTPUT); // A3=17, A8=22, A4=18
    digitalWrite(PIN_A4, LOW);

    {
        // read RTC
        time_t timestamp = Teensy3Clock.get();
        // write to time lib
        setTime(timestamp);
    }

    serialIO.setup();
    fileWriter.setupSpi();

    if(fileWriter.setupSdCard())
        onSdCardActive();
    else
        Serial.println("(W) Unable to access the SD card");

    audio.setup(config.audio);

    config.analyzer.signalSampleRate = config.audio.sampleRate;
    config.analyzer.fftWidth = AudioSystem::fftWidth;
    config.analyzer.isIqMeasurement = config.audio.isIqMeasurement;
    signalAnalyzer.setup(config.analyzer);
}

void loop()
{
    if(not canWriteData)
    {
        delay(1000);

        if(fileWriter.setupSdCard())
            onSdCardActive();

        return;
    }

    if(not audio.hasData())
    {
        delay(1);
        return;
    }

    static size_t sampleCounter = 0;
    static size_t sendSampleCounter = 0;
    sampleCounter++;

    serialIO.processInputs(config.audio);
    if(config.audio.hasChanges)
    {
        audio.updateIQ(config.audio);
        config.audio.hasChanges = false;
    }

    results.timestamp = millis();
    audio.extractSpectrum(results.spectrum);

    serialIO.onLoop(config);
    if(SerialUSB1.dtr())
    {
        serialIO.sendData(
            static_cast<char const*>((void*)results.spectrum.data()),
            results.spectrum.size() * 4,
            results.timestamp,
            sendSampleCounter);
        sendSampleCounter++;
    }
    else
    {
        sendSampleCounter = 0;
    }

    signalAnalyzer.processData(results);

    if(config.writeDataToSdCard)
    {
        bool ok = false;

        if(config.writeRawData)
        {
            ok = fileWriter.writeRawData(results, config);
            if(not ok)
            {
                Serial.println("(E) Failed to write raw data to SD card");
                canWriteData = false;
                // there does not seem to be a sane way to check if the SD card is back again - hope for the best
            }
        }

        if(config.writeCsvMetricsData)
        {
            ok = fileWriter.writeCsvMetricsData(results, config);
            if(not ok)
            {
                Serial.println("(E) Failed to write csv metrics data to SD card");
                canWriteData = false;
                // there does not seem to be a sane way to check if the SD card is back again - hope for the best
            }
        }

        if(config.writeCsvCarData)
        {
            ok = fileWriter.writeCsvCarData(results, config);
            if(not ok)
            {
                Serial.println("(E) Failed to write csv car data to SD card");
                canWriteData = false;
                // there does not seem to be a sane way to check if the SD card is back again - hope for the best
            }
        }

        // SerialUSB1.println(second());
        // SerialUSB1.print("csv sd write time: ");
    }
}

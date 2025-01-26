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

enum class States
{
    Warmup,  // start here and ensure the battery has enough power; wait 10s until starting to write to files
    Idle,    // do not analyze signal and do not write output
    Running, // write to one or both of serial and SD card
};

States state = States::Warmup;
AudioSystem audio;
SignalAnalyzer signalAnalyzer;
SignalAnalyzer::Results results; // global to optimize for speed
Config config;

FileIO fileWriter;
SerialIO serialIO;

bool hasSdCard = false;

void onSdCardActive()
{
    hasSdCard = true;
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
    serialIO.onLoop(config);
    if(config.audio.hasChanges)
    {
        audio.updateIQ(config.audio);
        config.audio.hasChanges = false;
    }

    auto now = millis();
    static auto lastTry = now;
    if(hasSdCard == false && (now - lastTry > 7000)) // 7s to have ~10s between attempts
    {
        // Note: This call can take about 3 seconds and it blocks!
        Serial.print("(W) Checking for SD card ... ");
        if(fileWriter.setupSdCard())
        {
            Serial.println("done");
            onSdCardActive();
        }
        else
        {
            Serial.println("not available");
        }
        lastTry = millis();
    }

    bool const hasSerial = SerialUSB1.dtr();

    switch(state)
    {
        case States::Warmup:
            if(millis() > 10000)
            {
                state = States::Idle;
                Serial.println("(I) Leaving Warmup");
                return;
            }
            break;

        case States::Idle:
            if(hasSerial || hasSdCard)
            {
                state = States::Running;
                Serial.println("(I) Leaving Idle");
                return;
            }
            break;

        case States::Running:
            if(not hasSerial && not hasSdCard)
            {
                state = States::Idle;
                Serial.println("(I) Enter Idle");
                return;
            }
            break;
    }

    if(not audio.hasData())
    {
        delay(1);
        return;
    }

    if(state == States::Idle)
    {
        delay(100);
        return;
    }

    results.timestamp = millis();
    audio.copySpectrumTo(results.spectrum);

    static size_t sampleCounter = 0;
    static size_t sendSampleCounter = 0;
    sampleCounter++;
    if(hasSerial)
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

    if(state == States::Running && hasSdCard && config.writeDataToSdCard)
    {
        bool ok = false;

        if(config.writeRawData)
        {
            ok = fileWriter.writeRawData(results, config);
            if(not ok)
            {
                Serial.println("(E) Failed to write raw data to SD card");
                hasSdCard = false;
            }
        }

        if(config.writeCsvMetricsData)
        {
            ok = fileWriter.writeCsvMetricsData(results, config);
            if(not ok)
            {
                Serial.println("(E) Failed to write csv metrics data to SD card");
                hasSdCard = false;
            }
        }

        if(config.writeCsvCarData)
        {
            ok = fileWriter.writeCsvCarData(results, config);
            if(not ok)
            {
                Serial.println("(E) Failed to write csv car data to SD card");
                hasSdCard = false;
            }
        }
    }
}

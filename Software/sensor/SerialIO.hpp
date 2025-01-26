#ifndef SERISALIO_HPP
#define SERISALIO_HPP

#include "AudioSystem.h"
#include "Config.hpp"
#include "SignalAnalyzer.hpp"

/**
 * @brief The SerialIO class provides means to communicate with the FFT_visualisation pde java code program
 */
class SerialIO
{
  public:
    void setup();

    void processInputs(AudioSystem::Config& config);
    void sendData(char const* data, size_t size, size_t timestamp, size_t counter);

    void onLoop(Config& config);

  private:
    bool hadConnection = false;
};

#endif

#pragma once

#include "DspModule.h"
#include <vector>
#include <memory>

namespace nexus {

class ProcessorChain {
public:
    ProcessorChain();
    ~ProcessorChain();

    // Initialize the chain with all modules
    void init();

    // Process a block of 16-bit PCM audio from JNI
    void process(int16_t* pcmBuffer, int numSamples, int sampleRate);

    // Get the global parameters object to update parameters from UI
    DspParameters& getParameters();

private:
    std::vector<std::unique_ptr<DspModule>> modules;
    DspParameters parameters;
    
    // Internal float buffer for processing to avoid allocations
    std::vector<float> floatBuffer;
};

} // namespace nexus

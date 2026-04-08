#include "ProcessorChain.h"
#include "modules/Modules.h"
#include <android/log.h>

#define LOG_TAG "DspNative"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

namespace nexus {

ProcessorChain::ProcessorChain() {
    init();
}

ProcessorChain::~ProcessorChain() {
}

void ProcessorChain::init() {
    modules.clear();
    // Add modules in the exact order specified in Phase 2
    modules.push_back(std::make_unique<PreEqModule>());
    modules.push_back(std::make_unique<MidSideModule>());
    modules.push_back(std::make_unique<PhaseAlignmentModule>());
    modules.push_back(std::make_unique<CrossfeedModule>());
    modules.push_back(std::make_unique<StereoWidthModule>());
    modules.push_back(std::make_unique<PanningModule>());
    modules.push_back(std::make_unique<HaasDelayModule>());
    modules.push_back(std::make_unique<HrtfModule>());
    modules.push_back(std::make_unique<DistanceModelingModule>());
    modules.push_back(std::make_unique<EarlyReflectionsModule>());
    modules.push_back(std::make_unique<ReverbModule>());

    LOGD("ProcessorChain initialized with %zu modules", modules.size());
}

DspParameters& ProcessorChain::getParameters() {
    return parameters;
}

void ProcessorChain::process(int16_t* pcmBuffer, int numSamples, int sampleRate) {
    if (parameters.get(GLOBAL_BYPASS) > 0.5f) {
        return; // Complete bypass
    }

    int numFrames = numSamples / 2; // Stereo assumed
    
    // Resize float buffer if needed (numSamples floats)
    if (floatBuffer.size() < static_cast<size_t>(numSamples)) {
        floatBuffer.resize(numSamples);
    }
    
    // Convert int16_t to float (-1.0 to 1.0)
    for (int i = 0; i < numSamples; i++) {
        floatBuffer[i] = static_cast<float>(pcmBuffer[i]) / 32768.0f;
    }
    
    // Run the chain
    float globalGain = parameters.get(GLOBAL_GAIN);
    if (globalGain != 1.0f) {
        for (int i = 0; i < numSamples; i++) {
            floatBuffer[i] *= globalGain;
        }
    }

    for (auto& module : modules) {
        module->process(floatBuffer.data(), numFrames, parameters, sampleRate);
    }
    
    // Convert back to int16_t with hard clipping
    for (int i = 0; i < numSamples; i++) {
        float sample = floatBuffer[i];
        if (sample > 1.0f) sample = 1.0f;
        else if (sample < -1.0f) sample = -1.0f;
        
        pcmBuffer[i] = static_cast<int16_t>(sample * 32767.0f);
    }
}

} // namespace nexus

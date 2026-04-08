#pragma once

#include "DspContext.h"
#include <string>

namespace nexus {

class DspModule {
public:
    virtual ~DspModule() = default;

    // Buffer contains interleaved stereo float audio (L R L R ...)
    // numFrames is the number of stereo pairs (so buffer contains numFrames * 2 elements)
    virtual void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) = 0;
    
    // Reset internal state (filters, delay lines, etc.)
    virtual void reset() = 0;
    
    virtual std::string getName() const = 0;
};

} // namespace nexus

#pragma once

#include "../DspModule.h"
#include "../DspMath.h"

namespace nexus {

class PreEqModule : public DspModule {
public:
    PreEqModule();
    void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) override;
    void reset() override;
    std::string getName() const override { return "Pre-EQ"; }
private:
    Biquad eqLowL, eqLowR;
    Biquad eqPeakL, eqPeakR;
    Biquad eqHighL, eqHighR;
    int prevSampleRate = 0;
};

class MidSideModule : public DspModule {
public:
    void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) override;
    void reset() override {}
    std::string getName() const override { return "Mid/Side"; }
};

class PhaseAlignmentModule : public DspModule {
public:
    PhaseAlignmentModule() { reset(); }
    void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) override;
    void reset() override;
    std::string getName() const override { return "Phase Alignment"; }
private:
    DelayLine delayL, delayR;
};

class CrossfeedModule : public DspModule {
public:
    CrossfeedModule();
    void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) override;
    void reset() override;
    std::string getName() const override { return "Crossfeed"; }
private:
    Biquad lpL, lpR, hpL, hpR;
    DelayLine delayL, delayR;
    int prevSampleRate = 0;
};

class StereoWidthModule : public DspModule {
public:
    void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) override;
    void reset() override {}
    std::string getName() const override { return "Stereo Width"; }
};

class PanningModule : public DspModule {
public:
    void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) override;
    void reset() override {}
    std::string getName() const override { return "Panning"; }
};

class HaasDelayModule : public DspModule {
public:
    HaasDelayModule();
    void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) override;
    void reset() override;
    std::string getName() const override { return "Haas Delay"; }
private:
    DelayLine delayLine;
};

class HrtfModule : public DspModule {
public:
    HrtfModule();
    void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) override;
    void reset() override;
    std::string getName() const override { return "HRTF Simulation"; }
private:
    Biquad notchL, notchR, peakL, peakR;
    int prevSampleRate = 0;
};

class DistanceModelingModule : public DspModule {
public:
    DistanceModelingModule();
    void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) override;
    void reset() override;
    std::string getName() const override { return "Distance Modeling"; }
private:
    Biquad lpL, lpR;
    int prevSampleRate = 0;
};

class EarlyReflectionsModule : public DspModule {
public:
    EarlyReflectionsModule();
    void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) override;
    void reset() override;
    std::string getName() const override { return "Early Reflections"; }
private:
    DelayLine delay1L, delay2L, delay1R, delay2R;
};

class ReverbModule : public DspModule {
public:
    ReverbModule();
    void process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) override;
    void reset() override;
    std::string getName() const override { return "Reverb"; }
private:
    // Simple Schroeder/Freeverb components
    struct Comb {
        DelayLine dl;
        float feedback = 0.5f;
        float damp = 0.5f;
        float filterStore = 0.0f;
        
        inline float process(float input) {
            float out = dl.read(dl.size() - 1);
            filterStore = (out * (1.0f - damp)) + (filterStore * damp);
            dl.write(input + filterStore * feedback);
            return out;
        }
    };
    
    struct Allpass {
        DelayLine dl;
        float feedback = 0.5f;
        
        inline float process(float input) {
            float dlOut = dl.read(dl.size() - 1);
            float out = -input + dlOut;
            dl.write(input + dlOut * feedback);
            return out;
        }
    };

    Comb combsL[4], combsR[4];
    Allpass allpassesL[2], allpassesR[2];
};

} // namespace nexus

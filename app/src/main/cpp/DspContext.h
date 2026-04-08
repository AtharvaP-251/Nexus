#pragma once

#include <atomic>
#include <cmath>

namespace nexus {

enum DspParamId {
    // Global
    GLOBAL_BYPASS = 0,
    GLOBAL_GAIN,

    // Pre-EQ
    EQ_PRE_GAIN,
    EQ_LOW_SHELF_FREQ,
    EQ_LOW_SHELF_GAIN,
    EQ_PEAK_FREQ,
    EQ_PEAK_GAIN,
    EQ_PEAK_Q,
    EQ_HIGH_SHELF_FREQ,
    EQ_HIGH_SHELF_GAIN,

    // Mid/Side
    MS_MID_GAIN,
    MS_SIDE_GAIN,

    // Phase Alignment
    PHASE_DELAY_MS,
    PHASE_INVERT,

    // Crossfeed (Linkwitz-Riley style)
    CROSSFEED_MIX,
    CROSSFEED_CUTOFF,

    // Stereo Width
    WIDTH_AMOUNT,

    // Panning
    PAN_BALANCE, // -1.0 to 1.0

    // Haas Delay
    HAAS_DELAY_MS,
    HAAS_MIX,

    // Distance Modeling
    DISTANCE_AMOUNT,
    DISTANCE_ROLLOFF,

    // HRTF 
    HRTF_INTENSITY,

    // Early Reflections
    ER_DELAY_SPREAD_MS,
    ER_MIX,

    // Reverb
    REVERB_DECAY,
    REVERB_DAMPING,
    REVERB_ROOM_SIZE,
    REVERB_WET,
    REVERB_DRY,

    // Max number of params
    PARAM_MAX_COUNT
};

class DspParameters {
public:
    DspParameters() {
        for (int i = 0; i < PARAM_MAX_COUNT; i++) {
            params[i].store(0.0f);
        }
        
        // Defaults
        set(GLOBAL_GAIN, 1.0f);
        set(EQ_PRE_GAIN, 1.0f);
        
        set(EQ_LOW_SHELF_FREQ, 150.0f);
        set(EQ_LOW_SHELF_GAIN, 0.0f); // dB
        set(EQ_PEAK_FREQ, 1000.0f);
        set(EQ_PEAK_GAIN, 0.0f);      // dB
        set(EQ_PEAK_Q, 0.707f);
        set(EQ_HIGH_SHELF_FREQ, 6000.0f);
        set(EQ_HIGH_SHELF_GAIN, 0.0f); // dB
        
        set(MS_MID_GAIN, 1.0f);
        set(MS_SIDE_GAIN, 1.0f);
        
        set(PHASE_DELAY_MS, 0.0f);
        set(PHASE_INVERT, 0.0f); // 0 = false, 1.0 = true
        
        set(CROSSFEED_MIX, 0.0f); // 0 to 1
        set(CROSSFEED_CUTOFF, 700.0f);
        
        set(WIDTH_AMOUNT, 1.0f); // 1.0 = normal, >1 = wide
        
        set(PAN_BALANCE, 0.0f);
        
        set(HAAS_DELAY_MS, 15.0f);
        set(HAAS_MIX, 0.0f);
        
        set(DISTANCE_AMOUNT, 0.0f);
        set(DISTANCE_ROLLOFF, 0.5f);
        
        set(HRTF_INTENSITY, 0.0f);
        
        set(ER_DELAY_SPREAD_MS, 20.0f);
        set(ER_MIX, 0.0f);
        
        set(REVERB_DECAY, 0.5f);
        set(REVERB_DAMPING, 0.5f);
        set(REVERB_ROOM_SIZE, 0.5f);
        set(REVERB_WET, 0.0f);
        set(REVERB_DRY, 1.0f);
    }

    void set(int paramId, float value) {
        if (paramId >= 0 && paramId < PARAM_MAX_COUNT) {
            params[paramId].store(value, std::memory_order_relaxed);
        }
    }

    float get(int paramId) const {
        if (paramId >= 0 && paramId < PARAM_MAX_COUNT) {
            return params[paramId].load(std::memory_order_relaxed);
        }
        return 0.0f;
    }

private:
    std::atomic<float> params[PARAM_MAX_COUNT];
};

} // namespace nexus

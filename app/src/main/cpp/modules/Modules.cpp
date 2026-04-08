#include "Modules.h"
#include <algorithm>

namespace nexus {

// --- PreEqModule ---
PreEqModule::PreEqModule() { reset(); }

void PreEqModule::reset() {
    eqLowL.reset(); eqLowR.reset();
    eqPeakL.reset(); eqPeakR.reset();
    eqHighL.reset(); eqHighR.reset();
    prevSampleRate = 0;
}

void PreEqModule::process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) {
    float preGainDb = params.get(EQ_PRE_GAIN);
    float preGain = std::pow(10.0f, preGainDb / 20.0f);
    
    if (sampleRate != prevSampleRate) {
        eqLowL.setLowShelf(params.get(EQ_LOW_SHELF_FREQ), params.get(EQ_LOW_SHELF_GAIN), sampleRate);
        eqLowR.setLowShelf(params.get(EQ_LOW_SHELF_FREQ), params.get(EQ_LOW_SHELF_GAIN), sampleRate);
        eqPeakL.setPeaking(params.get(EQ_PEAK_FREQ), params.get(EQ_PEAK_Q), params.get(EQ_PEAK_GAIN), sampleRate);
        eqPeakR.setPeaking(params.get(EQ_PEAK_FREQ), params.get(EQ_PEAK_Q), params.get(EQ_PEAK_GAIN), sampleRate);
        eqHighL.setHighShelf(params.get(EQ_HIGH_SHELF_FREQ), params.get(EQ_HIGH_SHELF_GAIN), sampleRate);
        eqHighR.setHighShelf(params.get(EQ_HIGH_SHELF_FREQ), params.get(EQ_HIGH_SHELF_GAIN), sampleRate);
        prevSampleRate = sampleRate;
    } // In real usage we should update coefficients dynamically if params change.

    for (int i = 0; i < numFrames; ++i) {
        float l = buffer[i*2] * preGain;
        float r = buffer[i*2+1] * preGain;
        
        l = eqHighL.process(eqPeakL.process(eqLowL.process(l)));
        r = eqHighR.process(eqPeakR.process(eqLowR.process(r)));
        
        buffer[i*2] = l;
        buffer[i*2+1] = r;
    }
}

// --- MidSideModule ---
void MidSideModule::process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) {
    float mg = params.get(MS_MID_GAIN);
    float sg = params.get(MS_SIDE_GAIN);
    
    if (mg == 1.0f && sg == 1.0f) return;

    for (int i = 0; i < numFrames; ++i) {
        float l = buffer[i*2];
        float r = buffer[i*2+1];
        
        float mid = (l + r) * 0.5f * mg;
        float side = (l - r) * 0.5f * sg;
        
        buffer[i*2] = mid + side;
        buffer[i*2+1] = mid - side;
    }
}

// --- PhaseAlignmentModule ---
void PhaseAlignmentModule::reset() {
    delayL.reset(); delayR.reset();
}
void PhaseAlignmentModule::process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) {
    float delayMs = params.get(PHASE_DELAY_MS);
    bool invert = params.get(PHASE_INVERT) > 0.5f;

    if (delayMs <= 0.0f && !invert) return;

    int delayFrames = static_cast<int>(delayMs * sampleRate / 1000.0f);
    if (delayFrames > 0 && delayL.size() < delayFrames + 1) {
        delayL.init(sampleRate); // up to 1 sec
        delayR.init(sampleRate);
    }

    for (int i = 0; i < numFrames; ++i) {
        float l = buffer[i*2];
        float r = buffer[i*2+1];

        if (delayFrames > 0) {
            delayL.write(l); delayR.write(r);
            l = delayL.read(delayFrames);
            r = delayR.read(delayFrames);
        }

        if (invert) {
            r = -r; // Arbitrarily invert right channel for phase experiments
        }

        buffer[i*2] = l;
        buffer[i*2+1] = r;
    }
}

// --- CrossfeedModule ---
CrossfeedModule::CrossfeedModule() { reset(); }

void CrossfeedModule::reset() {
    lpL.reset(); lpR.reset();
    hpL.reset(); hpR.reset();
    delayL.reset(); delayR.reset();
    prevSampleRate = 0;
}

void CrossfeedModule::process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) {
    float mix = params.get(CROSSFEED_MIX);
    if (mix <= 0.0f) return;

    if (sampleRate != prevSampleRate) {
        float cutoff = params.get(CROSSFEED_CUTOFF);
        // Approximation of Linkwitz-Riley crossover
        lpL.setLowPass(cutoff, 0.707f, sampleRate);
        lpR.setLowPass(cutoff, 0.707f, sampleRate);
        
        delayL.init((int)(sampleRate * 0.005f)); // Max 5ms delay
        delayR.init((int)(sampleRate * 0.005f));
        prevSampleRate = sampleRate;
    }
    
    int delayFrames = static_cast<int>(0.2f * sampleRate / 1000.0f); // ~200us delay for crossfeed

    for (int i = 0; i < numFrames; ++i) {
        float l = buffer[i*2];
        float r = buffer[i*2+1];
        
        float filteredL = lpL.process(l);
        float filteredR = lpR.process(r);
        
        delayL.write(filteredL);
        delayR.write(filteredR);
        
        float crossL = delayR.read(delayFrames);
        float crossR = delayL.read(delayFrames);
        
        buffer[i*2] = l + crossL * mix;
        buffer[i*2+1] = r + crossR * mix;
    }
}

// --- StereoWidthModule ---
void StereoWidthModule::process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) {
    float width = params.get(WIDTH_AMOUNT);
    if (width == 1.0f) return;

    for (int i = 0; i < numFrames; ++i) {
        float l = buffer[i*2];
        float r = buffer[i*2+1];
        
        float mid = (l + r) * 0.5f;
        float side = (l - r) * 0.5f;
        
        side *= width;
        
        buffer[i*2] = mid + side;
        buffer[i*2+1] = mid - side;
    }
}

// --- PanningModule ---
void PanningModule::process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) {
    float pan = params.get(PAN_BALANCE); // -1 to 1
    if (pan == 0.0f) return;
    
    // Constant power panning
    float p = (pan + 1.0f) * M_PI / 4.0f; // range 0 to pi/2
    float gainL = std::cos(p);
    float gainR = std::sin(p);

    for (int i = 0; i < numFrames; ++i) {
        buffer[i*2] *= gainL;
        buffer[i*2+1] *= gainR;
    }
}

// --- HaasDelayModule ---
HaasDelayModule::HaasDelayModule() { reset(); }

void HaasDelayModule::reset() { delayLine.reset(); }

void HaasDelayModule::process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) {
    float haasDelayMs = params.get(HAAS_DELAY_MS);
    float mix = params.get(HAAS_MIX);
    
    if (mix <= 0.0f || haasDelayMs <= 0.0f) return;

    int delayFrames = static_cast<int>(haasDelayMs * sampleRate / 1000.0f);
    if (delayLine.size() < delayFrames + 1) {
        delayLine.init(static_cast<int>(50.0f * sampleRate / 1000.0f)); // up to 50ms
    }

    for (int i = 0; i < numFrames; ++i) {
        float l = buffer[i*2];
        float r = buffer[i*2+1];
        
        // Delay left, mix into right (or vice versa, typically we delay one side)
        delayLine.write(l);
        float delayedL = delayLine.read(delayFrames);
        
        // Very basic haas effect addition
        buffer[i*2+1] += delayedL * mix;
    }
}

// --- HrtfModule ---
HrtfModule::HrtfModule() { reset(); }

void HrtfModule::reset() {
    notchL.reset(); notchR.reset();
    peakL.reset(); peakR.reset();
    prevSampleRate = 0;
}

void HrtfModule::process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) {
    float intensity = params.get(HRTF_INTENSITY);
    if (intensity <= 0.0f) return;
    
    if (sampleRate != prevSampleRate) {
        // Fake spectral coloration typical of HRTF
        notchL.setPeaking(8000.0f, 2.0f, -6.0f, sampleRate);
        notchR.setPeaking(8000.0f, 2.0f, -6.0f, sampleRate);
        peakL.setPeaking(4000.0f, 1.0f, 3.0f, sampleRate);
        peakR.setPeaking(4000.0f, 1.0f, 3.0f, sampleRate);
        prevSampleRate = sampleRate;
    }
    
    for (int i = 0; i < numFrames; ++i) {
        float l = buffer[i*2];
        float r = buffer[i*2+1];
        
        float fl = peakL.process(notchL.process(l));
        float fr = peakR.process(notchR.process(r));
        
        buffer[i*2] = l + (fl - l) * intensity;
        buffer[i*2+1] = r + (fr - r) * intensity;
    }
}

// --- DistanceModelingModule ---
DistanceModelingModule::DistanceModelingModule() { reset(); }

void DistanceModelingModule::reset() {
    lpL.reset(); lpR.reset();
    prevSampleRate = 0;
}

void DistanceModelingModule::process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) {
    float dist = params.get(DISTANCE_AMOUNT);
    if (dist <= 0.0f) return;

    if (sampleRate != prevSampleRate) {
        float cutoff = 20000.0f - (15000.0f * dist);
        lpL.setLowPass(cutoff, 0.707f, sampleRate);
        lpR.setLowPass(cutoff, 0.707f, sampleRate);
        prevSampleRate = sampleRate;
    }
    
    float atten = 1.0f / (1.0f + params.get(DISTANCE_ROLLOFF) * dist * dist);
    
    for (int i = 0; i < numFrames; ++i) {
        buffer[i*2] = lpL.process(buffer[i*2]) * atten;
        buffer[i*2+1] = lpR.process(buffer[i*2+1]) * atten;
    }
}

// --- EarlyReflectionsModule ---
EarlyReflectionsModule::EarlyReflectionsModule() { reset(); }

void EarlyReflectionsModule::reset() {
    delay1L.reset(); delay2L.reset();
    delay1R.reset(); delay2R.reset();
}

void EarlyReflectionsModule::process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) {
    float mix = params.get(ER_MIX);
    if (mix <= 0.0f) return;

    float spreadMs = params.get(ER_DELAY_SPREAD_MS);
    if (delay1L.size() == 0) {
        int maxDelay = static_cast<int>(100.0f * sampleRate / 1000.0f);
        delay1L.init(maxDelay); delay2L.init(maxDelay);
        delay1R.init(maxDelay); delay2R.init(maxDelay);
    }
    
    int t1L = static_cast<int>(spreadMs * 0.3f * sampleRate / 1000.0f);
    int t2L = static_cast<int>(spreadMs * 0.7f * sampleRate / 1000.0f);
    int t1R = static_cast<int>(spreadMs * 0.4f * sampleRate / 1000.0f);
    int t2R = static_cast<int>(spreadMs * 0.9f * sampleRate / 1000.0f);

    for (int i = 0; i < numFrames; ++i) {
        float l = buffer[i*2];
        float r = buffer[i*2+1];
        
        delay1L.write(l); delay2L.write(l);
        delay1R.write(r); delay2R.write(r);
        
        float erL = delay1L.read(t1L) * 0.6f + delay2L.read(t2L) * 0.4f;
        float erR = delay1R.read(t1R) * 0.6f + delay2R.read(t2R) * 0.4f;
        
        buffer[i*2] += erL * mix;
        buffer[i*2+1] += erR * mix;
    }
}

// --- ReverbModule ---
ReverbModule::ReverbModule() { reset(); }

void ReverbModule::reset() {
    for (int i=0; i<4; i++) { combsL[i].dl.reset(); combsR[i].dl.reset(); }
    for (int i=0; i<2; i++) { allpassesL[i].dl.reset(); allpassesR[i].dl.reset(); }
}

void ReverbModule::process(float* buffer, int numFrames, const DspParameters& params, int sampleRate) {
    float wet = params.get(REVERB_WET);
    if (wet <= 0.0f) return;

    float dry = params.get(REVERB_DRY);
    float decay = params.get(REVERB_DECAY);
    float damping = params.get(REVERB_DAMPING);
    float size = params.get(REVERB_ROOM_SIZE); // Impacts delay lengths

    if (combsL[0].dl.size() == 0) {
        // Init delays based on sampleRate
        int baseComb = static_cast<int>(0.03f * sampleRate); // 30ms base
        combsL[0].dl.init(baseComb); combsR[0].dl.init(baseComb + 23);
        combsL[1].dl.init(baseComb + 111); combsR[1].dl.init(baseComb + 111 + 23);
        combsL[2].dl.init(baseComb + 313); combsR[2].dl.init(baseComb + 313 + 23);
        combsL[3].dl.init(baseComb + 431); combsR[3].dl.init(baseComb + 431 + 23);
        
        int baseAp = static_cast<int>(0.005f * sampleRate); // 5ms
        allpassesL[0].dl.init(baseAp); allpassesR[0].dl.init(baseAp + 23);
        allpassesL[1].dl.init(baseAp + 40); allpassesR[1].dl.init(baseAp + 40 + 23);
    }

    // Update params
    for (int i=0; i<4; i++) {
        combsL[i].feedback = decay; combsR[i].feedback = decay;
        combsL[i].damp = damping; combsR[i].damp = damping;
    }

    for (int i = 0; i < numFrames; ++i) {
        float l = buffer[i*2];
        float r = buffer[i*2+1];
        
        float outL = 0, outR = 0;
        
        // Parallel Combs
        for (int c=0; c<4; c++) {
            outL += combsL[c].process(l) * 0.25f;
            outR += combsR[c].process(r) * 0.25f;
        }
        
        // Series Allpasses
        for (int a=0; a<2; a++) {
            outL = allpassesL[a].process(outL);
            outR = allpassesR[a].process(outR);
        }
        
        buffer[i*2] = l * dry + outL * wet;
        buffer[i*2+1] = r * dry + outR * wet;
    }
}

} // namespace nexus

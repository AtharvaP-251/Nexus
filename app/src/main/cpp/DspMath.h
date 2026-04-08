#pragma once

#include <cmath>
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace nexus {

// --- Basic Biquad Filter ---
class Biquad {
public:
    Biquad() { reset(); }
    
    void reset() {
        x1 = x2 = y1 = y2 = 0.0f;
    }
    
    void setCoefficients(float b0, float b1, float b2, float a1, float a2) {
        this->b0 = b0; this->b1 = b1; this->b2 = b2;
        this->a1 = a1; this->a2 = a2;
    }
    
    void setLowShelf(float f0, float gainDb, float sampleRate) {
        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * M_PI * f0 / sampleRate;
        float alpha = std::sin(w0) / 2.0f * std::sqrt((A + 1.0f/A)*(1.0f/1.0f - 1.0f) + 2.0f); // approx with S=1
        
        float b0_ = A * ((A+1.0f) - (A-1.0f)*std::cos(w0) + 2.0f*std::sqrt(A)*alpha);
        float b1_ = 2.0f * A * ((A-1.0f) - (A+1.0f)*std::cos(w0));
        float b2_ = A * ((A+1.0f) - (A-1.0f)*std::cos(w0) - 2.0f*std::sqrt(A)*alpha);
        float a0_ = (A+1.0f) + (A-1.0f)*std::cos(w0) + 2.0f*std::sqrt(A)*alpha;
        float a1_ = -2.0f * ((A-1.0f) + (A+1.0f)*std::cos(w0));
        float a2_ = (A+1.0f) + (A-1.0f)*std::cos(w0) - 2.0f*std::sqrt(A)*alpha;
        
        setCoefficients(b0_/a0_, b1_/a0_, b2_/a0_, a1_/a0_, a2_/a0_);
    }

    void setHighShelf(float f0, float gainDb, float sampleRate) {
        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * M_PI * f0 / sampleRate;
        float alpha = std::sin(w0) / 2.0f * std::sqrt((A + 1.0f/A)*(1.0f/1.0f - 1.0f) + 2.0f); 
        
        float b0_ = A * ((A+1.0f) + (A-1.0f)*std::cos(w0) + 2.0f*std::sqrt(A)*alpha);
        float b1_ = -2.0f * A * ((A-1.0f) + (A+1.0f)*std::cos(w0));
        float b2_ = A * ((A+1.0f) + (A-1.0f)*std::cos(w0) - 2.0f*std::sqrt(A)*alpha);
        float a0_ = (A+1.0f) - (A-1.0f)*std::cos(w0) + 2.0f*std::sqrt(A)*alpha;
        float a1_ = 2.0f * ((A-1.0f) - (A+1.0f)*std::cos(w0));
        float a2_ = (A+1.0f) - (A-1.0f)*std::cos(w0) - 2.0f*std::sqrt(A)*alpha;
        
        setCoefficients(b0_/a0_, b1_/a0_, b2_/a0_, a1_/a0_, a2_/a0_);
    }

    void setPeaking(float f0, float Q, float gainDb, float sampleRate) {
        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * M_PI * f0 / sampleRate;
        float alpha = std::sin(w0) / (2.0f * Q);
        
        float a0_ = 1.0f + alpha / A;
        float b0_ = (1.0f + alpha * A) / a0_;
        float b1_ = (-2.0f * std::cos(w0)) / a0_;
        float b2_ = (1.0f - alpha * A) / a0_;
        float a1_ = (-2.0f * std::cos(w0)) / a0_;
        float a2_ = (1.0f - alpha / A) / a0_;
        
        setCoefficients(b0_, b1_, b2_, a1_, a2_);
    }
    
    void setLowPass(float f0, float Q, float sampleRate) {
        float w0 = 2.0f * M_PI * f0 / sampleRate;
        float alpha = std::sin(w0) / (2.0f * Q);
        
        float a0_ = 1.0f + alpha;
        float b0_ = ((1.0f - std::cos(w0)) / 2.0f) / a0_;
        float b1_ = (1.0f - std::cos(w0)) / a0_;
        float b2_ = ((1.0f - std::cos(w0)) / 2.0f) / a0_;
        float a1_ = (-2.0f * std::cos(w0)) / a0_;
        float a2_ = (1.0f - alpha) / a0_;
        
        setCoefficients(b0_, b1_, b2_, a1_, a2_);
    }

    inline float process(float in) {
        float out = b0*in + b1*x1 + b2*x2 - a1*y1 - a2*y2;
        x2 = x1; x1 = in;
        y2 = y1; y1 = out;
        return out;
    }
    
private:
    float b0, b1, b2, a1, a2;
    float x1, x2, y1, y2;
};

// --- Delay Line ---
class DelayLine {
public:
    DelayLine() : writeIndex(0) {}
    
    void init(int maxDelayFrames) {
        buffer.assign(maxDelayFrames, 0.0f);
        writeIndex = 0;
    }
    
    void reset() {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        writeIndex = 0;
    }
    
    inline float read(int delayFrames) {
        if (buffer.empty()) return 0.0f;
        int readIndex = writeIndex - delayFrames;
        if (readIndex < 0) readIndex += buffer.size();
        return buffer[readIndex];
    }
    
    inline void write(float sample) {
        if (buffer.empty()) return;
        buffer[writeIndex] = sample;
        writeIndex = (writeIndex + 1) % buffer.size();
    }
    
    inline int size() const { return buffer.size(); }
    
private:
    std::vector<float> buffer;
    int writeIndex;
};

} // namespace nexus

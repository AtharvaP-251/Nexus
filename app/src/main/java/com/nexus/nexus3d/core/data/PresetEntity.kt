package com.nexus.nexus3d.core.data

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "presets")
data class PresetEntity(
    @PrimaryKey(autoGenerate = true) val id: Long = 0,
    val name: String,
    val isFactoryPreset: Boolean = false,
    
    // Global
    val globalBypass: Boolean = false,
    val globalGain: Float = 1.0f,
    
    // Pre-EQ
    val eqPreGain: Float = 1.0f,
    val eqLowShelfFreq: Float = 150.0f,
    val eqLowShelfGain: Float = 0.0f,
    val eqPeakFreq: Float = 1000.0f,
    val eqPeakGain: Float = 0.0f,
    val eqPeakQ: Float = 0.707f,
    val eqHighShelfFreq: Float = 6000.0f,
    val eqHighShelfGain: Float = 0.0f,
    
    // Mid/Side
    val msMidGain: Float = 1.0f,
    val msSideGain: Float = 1.0f,
    
    // Phase Alignment
    val phaseDelayMs: Float = 0.0f,
    val phaseInvert: Boolean = false,
    
    // Crossfeed
    val crossfeedMix: Float = 0.0f,
    val crossfeedCutoff: Float = 700.0f,
    
    // Stereo Width
    val widthAmount: Float = 1.0f,
    
    // Panning
    val panBalance: Float = 0.0f,
    
    // Haas Delay
    val haasDelayMs: Float = 15.0f,
    val haasMix: Float = 0.0f,
    
    // Distance Modeling
    val distanceAmount: Float = 0.0f,
    val distanceRolloff: Float = 0.5f,
    
    // HRTF
    val hrtfIntensity: Float = 0.0f,
    
    // Early Reflections
    val erDelaySpreadMs: Float = 20.0f,
    val erMix: Float = 0.0f,
    
    // Reverb
    val reverbDecay: Float = 0.5f,
    val reverbDamping: Float = 0.5f,
    val reverbRoomSize: Float = 0.5f,
    val reverbWet: Float = 0.0f,
    val reverbDry: Float = 1.0f
)

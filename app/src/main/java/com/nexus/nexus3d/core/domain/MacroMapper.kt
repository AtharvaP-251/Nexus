package com.nexus.nexus3d.core.domain

import com.nexus.nexus3d.DspNativeBridge

object MacroMapper {

    data class MacroValues(
        val width: Float = 0.5f,
        val depth: Float = 0.5f,
        val roomSize: Float = 0.5f,
        val clarity: Float = 0.5f,
        val distance: Float = 0.5f
    )

    fun applyMacros(macros: MacroValues) {
        // 1. Width
        // More width = less crossfeed, more side gain, more expansion
        val crossfeedMix = 0.6f * (1.0f - macros.width)
        val widthAmount = 1.0f + (macros.width * 2.0f)
        val sideGain = 1.0f + (macros.width * 0.5f)
        
        DspNativeBridge.setDspParameter(DspNativeBridge.CROSSFEED_MIX, crossfeedMix)
        DspNativeBridge.setDspParameter(DspNativeBridge.WIDTH_AMOUNT, widthAmount)
        DspNativeBridge.setDspParameter(DspNativeBridge.MS_SIDE_GAIN, sideGain)

        // 2. Depth
        // More depth = more Haas, more ER, more reverb wet, more distance modeling
        val haasMix = macros.depth * 0.7f
        val erMix = macros.depth * 0.6f
        val reverbWet = macros.depth * 0.5f
        val distanceAmount = macros.depth * 0.4f
        
        DspNativeBridge.setDspParameter(DspNativeBridge.HAAS_MIX, haasMix)
        DspNativeBridge.setDspParameter(DspNativeBridge.ER_MIX, erMix)
        DspNativeBridge.setDspParameter(DspNativeBridge.REVERB_WET, reverbWet)
        DspNativeBridge.setDspParameter(DspNativeBridge.DISTANCE_AMOUNT, distanceAmount)

        // 3. Room Size
        // Affects reverb decay, ER spread, distance intensity
        val reverbDecay = 0.1f + (macros.roomSize * 0.9f)
        val erSpread = 10.0f + (macros.roomSize * 90.0f)
        val distanceRolloff = 0.1f + (macros.roomSize * 0.9f)
        
        DspNativeBridge.setDspParameter(DspNativeBridge.REVERB_DECAY, reverbDecay)
        DspNativeBridge.setDspParameter(DspNativeBridge.ER_DELAY_SPREAD_MS, erSpread)
        DspNativeBridge.setDspParameter(DspNativeBridge.DISTANCE_ROLLOFF, distanceRolloff)

        // 4. Clarity
        // Less reverb damping, M/S balance (more center?), EQ tilt
        val reverbDamping = 1.0f - macros.clarity // more clarity = less damping
        val midGain = 1.0f + (macros.clarity * 0.3f)
        val highShelfGain = (macros.clarity - 0.5f) * 12.0f // -6dB to +6dB
        
        DspNativeBridge.setDspParameter(DspNativeBridge.REVERB_DAMPING, reverbDamping)
        DspNativeBridge.setDspParameter(DspNativeBridge.MS_MID_GAIN, midGain)
        DspNativeBridge.setDspParameter(DspNativeBridge.EQ_HIGH_SHELF_GAIN, highShelfGain)

        // 5. Distance
        // More distance = less dry, more HRTF, more attenuation
        val hrtfIntensity = macros.distance * 1.0f
        val reverbDry = 1.0f - (macros.distance * 0.7f)
        val globalGain = 1.0f - (macros.distance * 0.4f) // simulate air absorption/distance
        
        DspNativeBridge.setDspParameter(DspNativeBridge.HRTF_INTENSITY, hrtfIntensity)
        DspNativeBridge.setDspParameter(DspNativeBridge.REVERB_DRY, reverbDry)
        DspNativeBridge.setDspParameter(DspNativeBridge.GLOBAL_GAIN, globalGain)
    }
}

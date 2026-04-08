package com.nexus.nexus3d

import java.nio.ByteBuffer

object DspNativeBridge {
    init {
        System.loadLibrary("dsp_native")
    }

    // DSP Parameter IDs (matching DspContext.h)
    const val GLOBAL_BYPASS = 0
    const val GLOBAL_GAIN = 1
    const val EQ_PRE_GAIN = 2
    const val EQ_LOW_SHELF_FREQ = 3
    const val EQ_LOW_SHELF_GAIN = 4
    const val EQ_PEAK_FREQ = 5
    const val EQ_PEAK_GAIN = 6
    const val EQ_PEAK_Q = 7
    const val EQ_HIGH_SHELF_FREQ = 8
    const val EQ_HIGH_SHELF_GAIN = 9
    const val MS_MID_GAIN = 10
    const val MS_SIDE_GAIN = 11
    const val PHASE_DELAY_MS = 12
    const val PHASE_INVERT = 13
    const val CROSSFEED_MIX = 14
    const val CROSSFEED_CUTOFF = 15
    const val WIDTH_AMOUNT = 16
    const val PAN_BALANCE = 17
    const val HAAS_DELAY_MS = 18
    const val HAAS_MIX = 19
    const val DISTANCE_AMOUNT = 20
    const val DISTANCE_ROLLOFF = 21
    const val HRTF_INTENSITY = 22
    const val ER_DELAY_SPREAD_MS = 23
    const val ER_MIX = 24
    const val REVERB_DECAY = 25
    const val REVERB_DAMPING = 26
    const val REVERB_ROOM_SIZE = 27
    const val REVERB_WET = 28
    const val REVERB_DRY = 29

    /**
     * Set a specific DSP parameter
     */
    external fun setDspParameter(paramId: Int, value: Float)

    /**
     * Get a specific DSP parameter
     */
    external fun getDspParameter(paramId: Int): Float

    /**
     * Process audio directly using a DirectByteBuffer
     */
    external fun processAudio(byteBuffer: ByteBuffer, size: Int)
}

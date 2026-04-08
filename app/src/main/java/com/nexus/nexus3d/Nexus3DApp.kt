package com.nexus.nexus3d

import android.app.Application
import com.nexus.nexus3d.core.domain.DspRepository
import dagger.hilt.android.HiltAndroidApp
import javax.inject.Inject

@HiltAndroidApp
class Nexus3DApp : Application() {
    @Inject lateinit var dspRepository: DspRepository
}

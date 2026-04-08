package com.nexus.nexus3d.feature.library

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.nexus.nexus3d.core.data.LocalMediaScanner
import com.nexus.nexus3d.core.data.TrackDao
import com.nexus.nexus3d.core.data.TrackEntity
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.stateIn
import kotlinx.coroutines.launch
import javax.inject.Inject

import com.nexus.nexus3d.audio.AudioController

@HiltViewModel
class LibraryViewModel @Inject constructor(
    private val trackDao: TrackDao,
    private val scanner: LocalMediaScanner,
    private val audioController: AudioController
) : ViewModel() {

    val tracks: StateFlow<List<TrackEntity>> = trackDao.getAllTracks()
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            initialValue = emptyList()
        )

    fun scanLibrary() {
        viewModelScope.launch {
            scanner.scanDevice()
        }
    }
    
    fun playTrack(track: TrackEntity, allTracks: List<TrackEntity>) {
        val index = allTracks.indexOf(track).takeIf { it >= 0 } ?: 0
        audioController.playTracks(allTracks, index)
    }
}


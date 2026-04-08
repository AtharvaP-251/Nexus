package com.nexus.nexus3d.feature.player

import androidx.lifecycle.ViewModel
import com.nexus.nexus3d.audio.AudioController
import com.nexus.nexus3d.core.data.TrackEntity
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
class PlayerViewModel @Inject constructor(
    private val audioController: AudioController,
    private val dspRepository: com.nexus.nexus3d.core.domain.DspRepository
) : ViewModel() {

    val playbackState = audioController.playbackState
    val isDspEnabled = dspRepository.isDspEnabled

    fun playPause() = audioController.playPause()
    fun seekTo(position: Long) = audioController.seekTo(position)
    fun skipToNext() = audioController.skipToNext()
    fun skipToPrevious() = audioController.skipToPrevious()
    fun toggleShuffle() = audioController.toggleShuffle()
    fun toggleRepeatMode() = audioController.toggleRepeatMode()
    fun toggleDsp(enabled: Boolean) = dspRepository.toggleDsp(enabled)
    
    fun playTracks(tracks: List<TrackEntity>, startTrack: TrackEntity) {
        val index = tracks.indexOf(startTrack).takeIf { it >= 0 } ?: 0
        audioController.playTracks(tracks, index)
    }
}

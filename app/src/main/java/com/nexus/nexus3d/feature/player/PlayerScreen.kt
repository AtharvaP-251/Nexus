package com.nexus.nexus3d.feature.player

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.ArrowBack
import androidx.compose.material.icons.filled.ArrowForward
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun PlayerScreen(
    viewModel: PlayerViewModel = hiltViewModel(),
    onNavigateBack: () -> Unit
) {
    val playbackState by viewModel.playbackState.collectAsState()
    val track = playbackState.currentTrack
    
    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("Now Playing") },
                // Use a standard navigationIcon or simple text back button if needed
            )
        }
    ) { paddingValues ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(paddingValues)
                .padding(32.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center
        ) {
            // Album Art Placeholder
            Box(
                modifier = Modifier
                    .size(250.dp)
                    .clip(MaterialTheme.shapes.extraLarge)
                    .background(MaterialTheme.colorScheme.surfaceVariant)
            )

            Spacer(modifier = Modifier.height(32.dp))

            Text(
                text = track?.title ?: "No Track Selected",
                style = MaterialTheme.typography.headlineSmall,
                maxLines = 1,
                overflow = TextOverflow.Ellipsis,
                textAlign = TextAlign.Center
            )
            Text(
                text = track?.artist ?: "",
                style = MaterialTheme.typography.titleMedium,
                color = MaterialTheme.colorScheme.onSurfaceVariant,
                maxLines = 1,
                overflow = TextOverflow.Ellipsis
            )

            Spacer(modifier = Modifier.height(32.dp))

            // Seekbar
            var sliderPosition by remember(playbackState.progress) { mutableStateOf(playbackState.progress.toFloat()) }
            Slider(
                value = sliderPosition,
                onValueChange = { sliderPosition = it },
                onValueChangeFinished = {
                    viewModel.seekTo(sliderPosition.toLong())
                },
                valueRange = 0f..(playbackState.duration.toFloat().takeIf { it > 0 } ?: 100f),
                modifier = Modifier.fillMaxWidth()
            )

            Spacer(modifier = Modifier.height(16.dp))

            // Controls
            Row(
                horizontalArrangement = Arrangement.SpaceEvenly,
                verticalAlignment = Alignment.CenterVertically,
                modifier = Modifier.fillMaxWidth()
            ) {
                IconButton(onClick = { viewModel.skipToPrevious() }, modifier = Modifier.size(64.dp)) {
                    Icon(imageVector = Icons.Default.ArrowBack, contentDescription = "Previous", modifier = Modifier.size(48.dp))
                }
                
                FilledIconButton(
                    onClick = { viewModel.playPause() },
                    modifier = Modifier.size(80.dp),
                    shape = CircleShape
                ) {
                    // Quick check if playing, unfortunately there's no built-in pause icon in material-icons core, using standard compose painter or text later
                    // Using text for simplicity if standard icons are missing. Actually standard compose has filled.PlayArrow but maybe not Pause.
                    Text(if (playbackState.isPlaying) "||" else "▶", style = MaterialTheme.typography.headlineMedium)
                }

                IconButton(onClick = { viewModel.skipToNext() }, modifier = Modifier.size(64.dp)) {
                    Icon(imageVector = Icons.Default.ArrowForward, contentDescription = "Next", modifier = Modifier.size(48.dp))
                }
            }
        }
    }
}

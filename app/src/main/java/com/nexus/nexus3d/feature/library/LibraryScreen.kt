package com.nexus.nexus3d.feature.library

import android.Manifest
import android.os.Build
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.animation.*
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.platform.LocalContext
import androidx.core.content.ContextCompat
import androidx.hilt.navigation.compose.hiltViewModel
import com.nexus.nexus3d.core.data.TrackEntity
import android.content.pm.PackageManager

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun LibraryScreen(
    viewModel: LibraryViewModel = hiltViewModel(),
    onTrackClick: (TrackEntity, List<TrackEntity>) -> Unit
) {
    val tracks by viewModel.tracks.collectAsState()
    
    val permissionToRequest = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
        Manifest.permission.READ_MEDIA_AUDIO
    } else {
        Manifest.permission.READ_EXTERNAL_STORAGE
    }

    var hasPermission by remember { mutableStateOf(false) }

    val permissionLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.RequestPermission(),
        onResult = { isGranted ->
            hasPermission = isGranted
            if (isGranted) {
                viewModel.scanLibrary()
            }
        }
    )

    val context = LocalContext.current
    
    LaunchedEffect(Unit) {
        val status = ContextCompat.checkSelfPermission(context, permissionToRequest)
        if (status == PackageManager.PERMISSION_GRANTED) {
            hasPermission = true
            viewModel.scanLibrary()
        } else {
            permissionLauncher.launch(permissionToRequest)
        }
    }

    Scaffold(
        topBar = {
            TopAppBar(title = { Text("Library") })
        },
        floatingActionButton = {
            if (hasPermission) {
                ExtendedFloatingActionButton(
                    onClick = { viewModel.scanLibrary() },
                    text = { Text("Scan") },
                    icon = { }
                )
            }
        }
    ) { paddingValues ->
        if (!hasPermission) {
            Box(modifier = Modifier.fillMaxSize().padding(paddingValues), contentAlignment = Alignment.Center) {
                Text("Storage permission is required to scan library.")
            }
        } else if (tracks.isEmpty()) {
            Box(modifier = Modifier.fillMaxSize().padding(paddingValues), contentAlignment = Alignment.Center) {
                Text("No tracks found. Tap Scan to search.")
            }
        } else {
            LazyColumn(
                contentPadding = paddingValues,
                modifier = Modifier.fillMaxSize()
            ) {
                items(tracks) { track ->
                    TrackItem(track = track) {
                        onTrackClick(track, tracks)
                    }
                }
            }
        }
    }
}

@Composable
fun TrackItem(track: TrackEntity, onClick: () -> Unit) {
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .clickable(onClick = onClick)
            .padding(16.dp)
    ) {
        Text(
            text = track.title,
            style = MaterialTheme.typography.titleMedium,
            maxLines = 1,
            overflow = TextOverflow.Ellipsis
        )
        Text(
            text = "${track.artist} • ${track.album}",
            style = MaterialTheme.typography.bodyMedium,
            color = MaterialTheme.colorScheme.onSurfaceVariant,
            maxLines = 1,
            overflow = TextOverflow.Ellipsis
        )
    }
}

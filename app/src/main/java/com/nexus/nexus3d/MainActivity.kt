package com.nexus.nexus3d

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.currentBackStackEntryAsState
import androidx.navigation.compose.rememberNavController
import com.nexus.nexus3d.feature.library.LibraryScreen
import com.nexus.nexus3d.feature.library.LibraryViewModel
import androidx.hilt.navigation.compose.hiltViewModel
import com.nexus.nexus3d.feature.player.PlayerScreen
import dagger.hilt.android.AndroidEntryPoint

@AndroidEntryPoint
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            MaterialTheme {
                NexusApp()
            }
        }
    }
}

@Composable
fun NexusApp() {
    val navController = rememberNavController()
    Scaffold(
        bottomBar = {
            NavigationBar {
                val navBackStackEntry = navController.currentBackStackEntryAsState()
                val currentRoute = navBackStackEntry.value?.destination?.route

                NavigationBarItem(
                    selected = currentRoute == "library",
                    onClick = {
                        navController.navigate("library") {
                            popUpTo(navController.graph.startDestinationId) { saveState = true }
                            launchSingleTop = true
                            restoreState = true
                        }
                    },
                    icon = { Text("Lib") },
                    label = { Text("Library") }
                )
                
                NavigationBarItem(
                    selected = currentRoute == "player",
                    onClick = {
                        navController.navigate("player") {
                            popUpTo(navController.graph.startDestinationId) { saveState = true }
                            launchSingleTop = true
                            restoreState = true
                        }
                    },
                    icon = { Text("Play") },
                    label = { Text("Player") }
                )
            }
        }
    ) { paddingValues ->
        NavHost(
            navController = navController,
            startDestination = "library",
            modifier = Modifier.padding(paddingValues)
        ) {
            composable("library") {
                val viewModel: LibraryViewModel = hiltViewModel()
                LibraryScreen(
                    viewModel = viewModel,
                    onTrackClick = { startTrack, tracks ->
                        viewModel.playTrack(startTrack, tracks)
                        navController.navigate("player")
                    }
                )
            }
            composable("player") {
                PlayerScreen(
                    onNavigateBack = { navController.popBackStack() }
                )
            }
        }
    }
}

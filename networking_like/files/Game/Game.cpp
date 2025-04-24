#include "Game.h"

Game::Game() {

    // Initialise enet
    int enet_status = enet_initialize();
    Log::asserts(enet_status == 0, "An error occurred while initializing ENet.");
    atexit(enet_deinitialize); // Ensure ENet is deinitialized on exit

    // Setup raylib window
    const int screenWidth = 720;
    const int screenHeight = 480;

    window = raylib::Window(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");

}

void Game::begin() {
	while (!window.ShouldClose()) {
		update();
	}
}
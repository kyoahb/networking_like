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

	// Setup main menu
	std::shared_ptr<MainMenu> mainMenu = std::make_shared<MainMenu>();
	mainMenu->onHostButtonClick = [this](const std::string& address, int port) {
		// Make server
		server = std::make_unique<Server>(address, port);
		server->start();

		// Also make client
		client = std::make_unique<Client>();
		client->connect(address, port, "Client");
		};

	mainMenu->onJoinButtonClick = [this](const std::string& address, int port) {
		// Make client
		client = std::make_unique<Client>();
		client->connect(address, port, "Client");
		};

	mainMenu->onExitButtonClick = [this]() {
		// Exit the game
		window.Close();
		};
	stateManager.addState("MainMenu", mainMenu);
}

Game::~Game()
{
	// Destructor implementation
	if (server) {
		server->stop();
	}
	if (client) {
		client->disconnect();
	}
}

void Game::begin() {
	// Add main menu state
	stateManager.setState("MainMenu");

	while (!window.ShouldClose()) {
		update();
	}
}

void Game::update() {
	Input::update();

	// Draw
	BeginDrawing();
	window.ClearBackground(WHITE);

	stateManager.update();
	stateManager.draw();

	EndDrawing();

}
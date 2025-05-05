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
	std::shared_ptr<MainMenu> mainMenu = std::make_shared<MainMenu>(*this);
	mainMenu->onHostButtonClick = [this](const std::string& address, int port) {
		// Make server
		server = std::make_shared<Server>(address, port);
		server->start();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// Also make client
		client = std::make_shared<Client>();
		client->connect(address, port, "Client").wait();
		};

	mainMenu->onJoinButtonClick = [this](const std::string& address, int port) {
		// Make client
		client = std::make_shared<Client>();
		client->connect(address, port, "Client");
		};

	mainMenu->onExitButtonClick = [this]() {
		// Exit the game
		window.Close();
		};
	stateManager.addState("MainMenu", mainMenu);


	// Setup lobby
	std::shared_ptr<Lobby> lobby = std::make_shared<Lobby>(*this);
	stateManager.addState("Lobby", lobby);
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

	stateManager.draw();

	EndDrawing();

}
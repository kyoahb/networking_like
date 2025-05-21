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
	rlImGuiSetup(true); // Setup Raylib ImGUI connection

	// Setup main menu
	std::shared_ptr<MainMenu> mainMenu = std::make_shared<MainMenu>(*this);
	
	mainMenu->onExitButtonClick = [this]() {
		// Exit the game
		window.Close();
		CloseWindow();
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
	rlImGuiShutdown();
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
	BeginDrawing(); // Tell Raylib we are going to draw
	
	window.ClearBackground(WHITE); // Clear previous frame by replacing with full white
	rlImGuiBegin(); // Start Raylib-ImGUI frame

	stateManager.draw(); // Draw whatever current state is

	rlImGuiEnd(); // End Raylib-ImGUI frame
	EndDrawing(); // Tell Raylib we are done drawing

}

void Game::create_server(const std::string& address, int port) {
	// Create server
	if (server == nullptr) {
		server = std::make_shared<Server>(address, port);
	}
	server->start();
}

void Game::create_client(const std::string& address, int port) {
	// Create client
	if (client == nullptr) {
		client = std::make_shared<Client>();
	}
	client->connect(address, port, "Client").wait();
}
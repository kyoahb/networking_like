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

	// Setup World state
	std::shared_ptr<WorldState> worldState = std::make_shared<WorldState>(*this);
	stateManager.addState("WorldState", worldState);
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
	
	window.ClearBackground(GRAY); // Clear previous frame by replacing with full white
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

bool Game::is_local_server() const {
	// Check if local program is hosting a server, which the local client is also connected to
	if (server != nullptr && client != nullptr) {
		// Check if client is connected to the server
		return client->is_connected() && client->peers.get_server().handle == server->peers.get_self().handle;
	}
	return false;
}

void Game::destroy_server() {
	if (server) {
		server->prepare_destroy();
		server.reset(); // Should be the final reference to server removed, server will now be destroyed by shared_ptr
		server = nullptr;
	}
}

void Game::destroy_client() {
	if (client) {
		client->prepare_destroy();
		client.reset();
		client = nullptr;
	}
}
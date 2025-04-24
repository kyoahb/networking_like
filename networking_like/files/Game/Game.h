#pragma once

#include "Utils/Imports/common.h"
#include "Networking/Server/Server.h"
#include "Networking/Client/Client.h"
#include "Game/States/GameStateManager.h"
#include "Game/States/MainMenu.h"
#include "Utils/Input/Input.h"

class Game {
public:
	Game();
	~Game();
	void begin();
	
private:
	void update();
	
	raylib::Window window;
	std::unique_ptr<Server> server;
	std::unique_ptr<Client> client;

	GameStateManager stateManager;
};
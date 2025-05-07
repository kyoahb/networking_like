#pragma once

#include "Utils/Imports/common.h"
#include "Game/States/GameState.h"

class MainMenu : public GameState {
public:
	MainMenu(Game& _game);
	~MainMenu();

	void on_draw() override;

	std::function<void(const std::string& address, int port)> onHostButtonClick;
	std::function<void(const std::string& address, int port)> onJoinButtonClick;
	std::function<void()> onExitButtonClick;

};


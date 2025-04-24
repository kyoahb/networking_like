#pragma once

#include "Utils/Imports/common.h"
#include "Game/States/GameState.h"

class MainMenu : public GameState {
public:
	MainMenu();
	~MainMenu();

	void update() override;
	void draw() override;

	std::function<void(const std::string& address, int port)> onHostButtonClick;
	std::function<void(const std::string& address, int port)> onJoinButtonClick;
	std::function<void()> onExitButtonClick;

private:
	rgc::Label titleLabel;
	rgc::Button hostButton;
	rgc::Button joinButton;
	rgc::Button exitButton;

};


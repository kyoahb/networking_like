#pragma once
#include "Utils/Imports/common.h"
#include "Game/States/GameState.h"

class GameStateManager {
public:
	std::unordered_map<std::string, std::shared_ptr<GameState>> states;
	std::shared_ptr<GameState> current_state;

	void setState(const std::string& state);
	void addState(const std::string& state_name, std::shared_ptr<GameState> state);
	void update();
	void draw();
};
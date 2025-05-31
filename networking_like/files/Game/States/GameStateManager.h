#pragma once
#include "Utils/Imports/common.h"
#include "Game/States/GameState.h"

class GameStateManager {
public:
	std::unordered_map<std::string, std::shared_ptr<GameState>> states;
	std::shared_ptr<GameState> current_state;

	bool setState(const std::string& state); // F if could not find/set, T if set
	bool addState(const std::string& state_name, std::shared_ptr<GameState> state); // F if could not add (name already exists), T if added
	void draw();
};
#pragma once
#include "Utils/Imports/common.h"
#include "Game/States/GameState.h"

class GameStateManager {
public:
	std::unordered_map<std::string, std::shared_ptr<GameState>> states;
	std::shared_ptr<GameState> current_state;

	bool setState(const std::string& state); // F if could not find/set, T if set
	bool addState(const std::string& state_name, std::shared_ptr<GameState> state); // F if could not add (name already exists), T if added
	
	
	/* 
	* @brief Force a state to be set. If the state already exists, it will be used. If not, it will be added and then used.
	* 
	* This uses a lambda factory function to generate the state. This allows the GameState shared_ptr to ONLY be generated if it does not already exist
	* Therefore, the lambda function passed in should be one that just returns a std::make_shared<GameState>();
	*/
	void forceState(const std::string& state_name, std::function<std::shared_ptr<GameState>()> factory); // If state exists already, use that. If not, add state and then use it.
	void draw();
};
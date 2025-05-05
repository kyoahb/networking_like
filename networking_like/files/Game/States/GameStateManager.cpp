#include "GameStateManager.h"

void GameStateManager::setState(const std::string& state) {
	// Deactivate previous state
	if (current_state) {
		current_state->deactivate();
	}

	// Find new state in states list
	auto it = states.find(state);
	if (it != states.end()) {
		current_state = it->second;
		current_state->activate();
	}
	else {
		Log::error("State not found: " + state);
	}
}

void GameStateManager::addState(const std::string& state_name, std::shared_ptr<GameState> state) {
	states[state_name] = state;
}

void GameStateManager::draw() {
	if (current_state) {
		current_state->draw();
	}
}
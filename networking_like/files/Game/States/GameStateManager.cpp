#include "GameStateManager.h"

bool GameStateManager::setState(const std::string& state) {
	// Find new state in states list
	auto it = states.find(state);
	if (it != states.end()) {
		if (current_state) { // Deactivate old state if it exists
			current_state->deactivate();
		}
		current_state = it->second;
		current_state->activate();
		return true;
	}
	return false;
	
}

bool GameStateManager::addState(const std::string& state_name, std::shared_ptr<GameState> state) {
	if (states.find(state_name) != states.end()) {
		Log::error("State already exists: " + state_name);
		return false; // State with the same name already exists
	}
	states[state_name] = state;
	return true;
}

void GameStateManager::forceState(const std::string& state_name, std::function<std::shared_ptr<GameState>()> factory) {
	// If the state already exists, use that one
	if (setState(state_name)) {
		// Successfully set existing state
		return;
	}

	// State doesn't exist — create and add it
	auto new_state = factory(); // Only constructed if needed
	addState(state_name, new_state);
	setState(state_name); // Assume no errors here. May need to add error-checking in the future.
}

void GameStateManager::draw() {
	if (current_state) {
		current_state->draw();
	}
}
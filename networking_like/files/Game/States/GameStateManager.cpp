#include "GameStateManager.h"
#include "Game/Events/EventList.h"

bool GameStateManager::setState(const std::string& state) {
	LOG_SCOPE_GAME;
	// Find new state in states list
	auto it = states.find(state);
	if (it != states.end()) {
		if (current_state) { // Deactivate old state if it exists
			current_state->deactivate();
		}
		current_state = it->second;
		current_state->activate();

		// Trigger event
		Log::trace("Changing state to " + state);
		Events::Game::StateChange::trigger(Events::Game::StateChangeData(state));

		return true;
	}
	return false;
	
}

bool GameStateManager::addState(const std::string& state_name, std::shared_ptr<GameState> state) {
	LOG_SCOPE_GAME;
	if (states.find(state_name) != states.end()) {
		Log::error("State already exists: " + state_name);
		return false; // State with the same name already exists
	}
	states[state_name] = state;
	return true;
}

void GameStateManager::draw() {
	if (current_state) {
		current_state->draw();
	}
}
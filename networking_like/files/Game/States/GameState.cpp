#include "GameState.h"
#include "Game/Game.h"

GameState::GameState(Game& _game, const std::string& _name) : game(_game), name(_name) {
	// Constructor implementation
}

GameState::~GameState() {
	// Destructor implementation
}

void GameState::draw() {
	// Draw the game state
}

void GameState::activate() {
	interactable = true;
}

void GameState::deactivate() {
	interactable = false;
}

bool GameState::isActive() const {
	return interactable;
}
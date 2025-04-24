#include "GameState.h"

GameState::GameState(const std::string& _name) : name(_name) {
	// Constructor implementation
}

GameState::~GameState() {
	// Destructor implementation
}

void GameState::update() {
	// Update the game state
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
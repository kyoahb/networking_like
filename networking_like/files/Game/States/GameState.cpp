#include "GameState.h"
#include "Game/Game.h"

GameState::GameState(Game& _game, const std::string& _name) : game(_game), name(_name) {
	// Constructor implementation
}

GameState::~GameState() {
	// Destructor implementation
}

void GameState::on_draw() {
	// Draw the game state
}

void GameState::on_activate() {
	// Activate the game state
}

void GameState::on_deactivate() {
	// Deactivate the game state
}

void GameState::draw() {
	on_draw();
}

void GameState::activate() {
	interactable = true;
	on_activate();
}

void GameState::deactivate() {
	interactable = false;
	on_deactivate();
}

bool GameState::isActive() const {
	return interactable;
}
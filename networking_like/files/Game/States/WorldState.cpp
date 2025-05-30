#include "WorldState.h"
#include "Game/Game.h"
WorldState::WorldState(Game& _game) : GameState(_game, "WorldState") {

}

WorldState::~WorldState() {
	// Destructor implementation
}

void WorldState::on_draw() {
	// Drawing logic for the world state
}

void WorldState::on_activate() {
	// Logic to execute when the world state is activated
	if (game.is_local_server()) {
		// We are going to tell the server to generate the world and send it to peers

	}
}

void WorldState::on_deactivate() {
	// Logic to execute when the world state is deactivated
}
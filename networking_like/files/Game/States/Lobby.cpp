#include "Lobby.h"

Lobby::Lobby() : GameState("Lobby") {
	backButton = rgc::Button(rgc::Bounds(raylib::Vector2(400, 390), raylib::Vector2(200, 50)), "Back");
}

Lobby::~Lobby() {
	// Destructor implementation
}

void Lobby::draw() {
	// Draw the lobby

}
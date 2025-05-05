#include "Lobby.h"
#include "Game/Game.h"

class LobbyMember {
public:
	std::string name;
	std::string address;
	int port;

	rgc::Label nameLabel;
	rgc::Label addressLabel;
	rgc::Label portLabel;

	LobbyMember(LocalNetPeer peer) : name(peer.handle), address("ADDRESS HERE"), port(1234) {};

	void draw() {
		nameLabel.Show();
		addressLabel.Show();
		portLabel.Show();

		nameLabel.Update();
		addressLabel.Update();
		portLabel.Update();
	}
};


Lobby::Lobby(Game& _game) : GameState(_game, "Lobby") {
	backButton = rgc::Button(rgc::Bounds(raylib::Vector2(400, 390), raylib::Vector2(200, 50)), "Back");
	backButton.OnClick([this]() {

		if (!game.stateManager.setState("MainMenu")) {
			Log::asserts(false, "Failed to set state to MainMenu from Lobby");
		}
	});
}

Lobby::~Lobby() {
	// Destructor implementation
}

void Lobby::on_draw() {
	// Draw the lobby

	backButton.Show();
	if (!interactable) return;

	backButton.Update();

}
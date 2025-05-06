#include "Lobby.h"
#include "Game/Game.h"

LobbyMember::LobbyMember(const std::string& _name, int _id, int y_offset) : name(_name), id(_id), y_offset(y_offset) {
    
	// Convert name and id to const char*
	cName = name.c_str();
	idStr = std::to_string(id);
	cId = idStr.c_str();


	nameLabel = rgc::Label(rgc::Bounds(raylib::Vector2(0, y_offset), raylib::Vector2(200, 300+ y_offset)), cName);
    idLabel = rgc::Label(rgc::Bounds(raylib::Vector2(300, y_offset), raylib::Vector2(500, 300 + y_offset)), cId);

}

void LobbyMember::draw() {
	nameLabel.Show();
	idLabel.Show();

	nameLabel.Update();
	idLabel.Update();
}

Lobby::Lobby(Game& _game) : GameState(_game, "Lobby") {
	backButton = rgc::Button(rgc::Bounds(raylib::Vector2(400, 390), raylib::Vector2(200, 50)), "Back");
	backButton.OnClick([this]() {

		if (!game.stateManager.setState("MainMenu")) {
			Log::asserts(false, "Failed to set state to MainMenu from Lobby");
		}
	});

	lobbyPanel = rgc::Panel(rgc::Bounds(raylib::Vector2(0, 0), raylib::Vector2(720, 480)), "Lobby Panel");

}

Lobby::~Lobby() {
	// Destructor implementation
}

void Lobby::on_activate() {
	if (!game.client) {
		Log::error("Client is not initialized");
		return;
	}

	// Self member
	LobbyMember selfMember(game.client->peers.get_self().handle, game.client->peers.get_self().id, 0);
	members.push_back(selfMember);

	int y_offset = 50;
	int offset_change = 200;
	for (auto peer : game.client->peers.get_peers()) {
		LobbyMember member(peer.handle, peer.id, y_offset);
		members.push_back(member);
		y_offset += offset_change;
	}
}

void Lobby::on_deactivate() {
	members.clear(); // Clear the members list when deactivating the lobby
}

void Lobby::on_draw() {
	// Draw the lobby

	lobbyPanel.Show();
	backButton.Show();

	for (auto& member : members) {
		member.draw();
	}

	if (!interactable) return;

	lobbyPanel.Update();
	backButton.Update();
}
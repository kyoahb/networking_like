#include "Lobby.h"
#include "Game/Game.h"

LobbyMember::LobbyMember(std::string _name, int _id, int _y_offset) : name(_name), idStr(std::to_string(_id)), y_offset(_y_offset) {

	nameLabel = rgc::Label(rgc::Bounds(raylib::Vector2(start_x, y_offset), raylib::Vector2(start_x+size_x, size_y+y_offset)), name.c_str());
	idLabel = rgc::Label(rgc::Bounds(raylib::Vector2(start_x+id_offset_x, y_offset), raylib::Vector2(start_x+id_offset_x+size_x, size_y + y_offset)), idStr.c_str());
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
	members.emplace_back(game.client->peers.get_self().handle, game.client->peers.get_self().id, 0);

	int y_offset = 50;
	int offset_change = 200;
	for (auto peer : game.client->peers.get_peers()) {
		members.emplace_back(peer.handle, peer.id, y_offset);
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
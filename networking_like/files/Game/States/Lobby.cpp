#include "Lobby.h"
#include "Game/Game.h"

LobbyMember::LobbyMember(std::string _name, int _id, int _y_offset) : name(_name), idStr(std::to_string(_id)), y_offset(_y_offset) {
}

void LobbyMember::draw() {
	ImGui::Text("%s", name.c_str());
	ImGui::SameLine();
	ImGui::Text("%s", idStr.c_str());
}

Lobby::Lobby(Game& _game) : GameState(_game, "Lobby") {

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

	if (ImGui::Button("Back")) {
		if (!game.stateManager.setState("MainMenu")) {
			Log::asserts(false, "Failed to set state to MainMenu from Lobby");
		}
	}
	for (auto& member : members) {
		member.draw();
	}
}
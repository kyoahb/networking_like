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

void Lobby::add_all_members() {
	int y_offset = initial_y_offset;
	for (auto peer : game.client->peers.get_peers()) {
		members.emplace_back(peer.handle, peer.id, y_offset);
		y_offset += offset_change;
	}
}

void Lobby::on_activate() {
	if (!game.client) {
		Log::error("Client is not initialized");
		return;
	}
	// Subscribe to events
	on_peer_added_callbackid = Events::Client::PeerAdded::register_callback([this](const Events::Client::PeerAddedData& data) {
		this->on_peer_added(data);
		});
	on_peer_removed_callbackid = Events::Client::PeerRemoved::register_callback([this](const Events::Client::PeerRemovedData& data) {
		this->on_peer_removed(data);
		});

	// Self member
	members.emplace_back(game.client->peers.get_self().handle, game.client->peers.get_self().id, 0);

	// Add members from peers list
	add_all_members();
}

void Lobby::on_deactivate() {
	// Unsubscribe from events
	Events::Client::PeerAdded::unregister_callback(on_peer_added_callbackid);
	Events::Client::PeerRemoved::unregister_callback(on_peer_removed_callbackid);

	members.clear(); // Clear the members list when deactivating the lobby
}

void Lobby::on_draw() {
	// Draw the lobby

	if (ImGui::Button("Back")) {
		// If we are a client, disconnect
		if (game.client) {
			game.client->disconnect().wait();
		}
		// If we are a server, stop the server
		if (game.server) {
			game.server->stop().wait();
		}

		// Set state to MainMenu
		if (!game.stateManager.setState("MainMenu")) {
			Log::asserts(false, "Failed to set state to MainMenu from Lobby");
		}
	}
	for (auto& member : members) {
		member.draw();
	}
}

void Lobby::on_peer_added(const Events::Client::PeerAddedData& data) {
	members.emplace_back(data.peer.handle, data.peer.id, initial_y_offset + members.size() * offset_change);
};

void Lobby::on_peer_removed(const Events::Client::PeerRemovedData& data) {
	// Just clear list and remake it
	members.clear();
	add_all_members();
}
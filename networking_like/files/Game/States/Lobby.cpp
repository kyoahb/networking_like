#include "Lobby.h"
#include "Game/Game.h"

LobbyMember::LobbyMember(std::string _name, int _id, int _y_offset) : name(_name), id(_id), y_offset(_y_offset) {
}

void LobbyMember::draw() {
	ImGui::Text("%s", name.c_str());
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string(id).c_str());

	if (id == 0) {
		ImGui::SameLine();
		ImGui::Text("HOST");
	}
}

LocalMember::LocalMember(LocalNetPeer self) : LobbyMember(self.handle, self.id, 0) {
	// Constructor implementation
}

void LocalMember::draw() {
	ImGui::Text("%s", name.c_str());
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string(id).c_str());
	ImGui::SameLine();
	ImGui::Text("You");

	if (id == 0) {
		ImGui::SameLine();
		ImGui::Text("HOST");
	}
}

Lobby::Lobby(Game& _game) : GameState(_game, "Lobby") {

}

Lobby::~Lobby() {
	// Destructor implementation
}

void Lobby::add_all_members() {
	// Add self
	self = LocalMember(game.client->peers.get_self());

	// Add peers
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

	if (game.server != nullptr) { // Check if user is host
		is_host = true;
	}
	else {
		is_host = false;
	}

	// Subscribe to events
	on_peer_added_callbackid = Events::Client::PeerAdded::register_callback([this](const Events::Client::PeerAddedData& data) {
		this->on_peer_added(data);
		});
	on_peer_removed_callbackid = Events::Client::PeerRemoved::register_callback([this](const Events::Client::PeerRemovedData& data) {
		this->on_peer_removed(data);
		});
	on_client_disconnected_callbackid = Events::Client::Disconnect::register_callback([this](const Events::Client::DisconnectData& data) {
		this->on_client_disconnected(data);
		});

	// Add members from peers list & self
	members.clear();
	add_all_members();
}

void Lobby::on_deactivate() {
	// Unsubscribe from events
	Events::Client::PeerAdded::unregister_callback(on_peer_added_callbackid);
	Events::Client::PeerRemoved::unregister_callback(on_peer_removed_callbackid);
	Events::Client::Disconnect::unregister_callback(on_client_disconnected_callbackid);

	members.clear(); // Clear the members list when deactivating the lobby

	
}

void Lobby::on_draw() {
	// Draw the lobby

	if (ImGui::Button("Back")) {
		// If we are a client, disconnect
		if (game.client) {
			game.client->prepare_destroy();
			game.client.reset(); // Should be the final reference to client removed, client will now be destroyed by shared_ptr
			game.client = nullptr;
		}
		// If we are a server, stop the server
		if (game.server) {
			game.server->prepare_destroy();
			game.server.reset(); // Should be the final reference to server removed, server will now be destroyed by shared_ptr
			game.server = nullptr;
		}

		// Set state to MainMenu
		if (!game.stateManager.setState("MainMenu")) {
			Log::asserts(false, "Failed to set state to MainMenu from Lobby");
		}
	}
	self.draw();
	for (auto& member : members) {
		member.draw();
		if (is_host) { // Only draw kick button if we are the host
			ImGui::SameLine();
			draw_kick_button(member);
		}
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

void Lobby::on_client_disconnected(const Events::Client::DisconnectData& data) {
	// Return to main menu
	if (!game.stateManager.setState("MainMenu")) {
		Log::asserts(false, "Failed to set state to MainMenu from Lobby");
	}
}

void Lobby::draw_kick_button(LobbyMember member) {
	Log::asserts(game.server != nullptr, "Server must exist to draw kick button");

	// Get ENetPeer* from member's handle
	auto peer = game.server->peers.get_peer(member.id);
	
	if (peer.has_value()) {

		ENetPeer* enet_peer = peer->peer;

		if (ImGui::Button("Kick", ImVec2(50, 20))) { // Kick Button
			game.server->disconnect_peer(enet_peer);
		}
	}
}
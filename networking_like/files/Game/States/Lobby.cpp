#include "Lobby.h"
#include "Game/Game.h"
#include "Networking/Packets/Data/ChangeState.h"
LobbyMember::LobbyMember(std::string _name, int _id, int _y_offset, bool _is_host, bool _is_local_user) : name(_name), id(_id), y_offset(_y_offset), is_host(_is_host), is_local_user(_is_local_user) {
}

LobbyMember::LobbyMember(LocalNetPeer peer) : name(peer.handle), id(peer.id), y_offset(0), is_host(peer.is_host), is_local_user(true) {
	// Constructor for LocalNetPeer
}

void LobbyMember::draw() {
	ImGui::Text("%s", name.c_str());
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string(id).c_str());

	if (is_local_user) {
		ImGui::SameLine();
		ImGui::Text("You");
	}

	if (is_host) {
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
	self = LobbyMember(game.client->peers.get_self());

	// Add peers
	int y_offset = initial_y_offset;
	for (auto peer : game.client->peers.get_peers()) {
		members.emplace_back(peer.handle, peer.id, y_offset, peer.is_host, false);
		y_offset += offset_change;
	}
}

void Lobby::on_activate() {
	if (!game.client) {
		Log::error("Client is not initialized");
		return;
	}

	if (game.is_local_server()) { // Check if user is host
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
	on_event_receive_callbackid = Events::Client::EventReceive::register_callback([this](const Events::Client::EventReceiveData& data) {
		const ENetEvent& event = data.event;
		if (event.type != ENET_EVENT_TYPE_RECEIVE || !event.packet) return;
		Packet p(event.packet);
		if (p.header.type != PacketType::CHANGE_STATE) return;
		if (p.header.subtype == CHANGE_STATE_TO) {
			ChangeStateTo change_state = SerializationUtils::deserialize<ChangeStateTo>(p.data, p.header.size);
			if (change_state.state_name == "WorldState")
				game.stateManager.setState("WorldState");
		}
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
	Events::Client::EventReceive::unregister_callback(on_event_receive_callbackid);

	members.clear(); // Clear the members list when deactivating the lobby

	
}

void Lobby::on_draw() {
	// Draw the lobby

	ImGui::Begin("Lobby", NULL,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);

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
		return;
	}

	if (!game.client) {
		return; // Client not initialised, likely means on_draw() was called in the middle of de-initialising client.
	}

	// Draw server name label
	ImGui::Text(game.client->peers.get_server().handle.c_str());

	// Draw members
	if (ImGui::BeginTable("LobbyTable", is_host ? 4 : 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Status");
		if (is_host) ImGui::TableSetupColumn("Action");
		ImGui::TableHeadersRow();

		// Draw self
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("%s", self.name.c_str());
		ImGui::TableSetColumnIndex(1); ImGui::Text("%d", self.id);
		ImGui::TableSetColumnIndex(2); ImGui::Text("You%s", self.is_host ? " (HOST)" : "");
		if (is_host) { ImGui::TableSetColumnIndex(3); ImGui::Text("N/A"); }

		// Draw other members
		for (const auto& member : members) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("%s", member.name.c_str());
			ImGui::TableSetColumnIndex(1); ImGui::Text("%d", member.id);
			ImGui::TableSetColumnIndex(2);
			if (member.is_host) ImGui::Text("HOST");
			else ImGui::Text("Client");
			if (is_host) {
				ImGui::TableSetColumnIndex(3);
				std::string kick_label = "Kick##" + std::to_string(member.id);
				if (ImGui::Button(kick_label.c_str())) {
					kick_member(member);
				}
			}
		}
		ImGui::EndTable();
	}

	if (is_host && game.server) {
		// Below this point is only visible for the host.
		static bool allow_connections = game.server->are_connections_allowed();
		if (ImGui::Checkbox("Allow New Connections", &allow_connections)) {
			if (allow_connections) game.server->allow_connections();
			if (!allow_connections) game.server->refuse_connections();
		}

		ImGui::SameLine();

		if (ImGui::Button("Start Game")) {
			// Tell all clients to go to World gamestate
			ChangeStateTo change_state;
			change_state.state_name = "WorldState";
			std::string serialized = SerializationUtils::serialize<ChangeStateTo>(change_state);

			Packet packet(
				PacketType::CHANGE_STATE,
				PacketDirection::SERVER_TO_CLIENT,
				ChangeStateType::CHANGE_STATE_TO,
				serialized.data(),
				serialized.size(),
				true
			);

			// Broadcast to all clients
			game.server->broadcast_packet(packet);
		}
	}

	ImGui::End();
	



}

void Lobby::on_peer_added(const Events::Client::PeerAddedData& data) {
	members.emplace_back(data.peer.handle, data.peer.id, initial_y_offset + members.size() * offset_change, data.peer.is_host, false);
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

void Lobby::kick_member(LobbyMember member) {
	Log::asserts(is_host == true && game.server != nullptr, "Server must exist to kick member");

	// Get ENetPeer* from member's handle
	auto peer = game.server->peers.get_peer(member.id);
	
	if (peer.has_value()) {

		ENetPeer* enet_peer = peer->peer;
		game.server->disconnect_peer(enet_peer);
	}
}
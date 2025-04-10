#include "ClientPeerlist.h"

ClientPeerlist::ClientPeerlist() {
}

bool ClientPeerlist::is_connected() {
	LOG_SCOPE_CLIENT;
	return connected;
}

void ClientPeerlist::clear() {
	LOG_SCOPE_CLIENT;
	if (!connected) {
		Log::error("ClientPeerlist is not connected, cannot clear");
		return;
	}
	peers.clear();
	self = LocalNetPeer();
	server = NetPeer();
	connected = false;
}

void ClientPeerlist::connect(ENetPeer* server_peer) {
	LOG_SCOPE_CLIENT;
	if (connected) {
		Log::warn("ClientPeerlist is already connected, cannot connect again");
		return;
	}
	connected = true;
	server.peer = server_peer;
	server.handle = static_cast<char*>(server_peer->data); // Temporary, server may prefer another handle
}

void ClientPeerlist::setup_by_confirmation(const ClientConnectConfirm& connection_confirmation) {
	LOG_SCOPE_CLIENT;

	if (!connected) {
		Log::error("ClientPeerlist is not connected, cannot setup by confirmation");
		return;
	}

	self.id = connection_confirmation.client_id;
	self.handle = connection_confirmation.client_decided_handle;


	server.handle = connection_confirmation.server_preferred_handle;
	for (const auto& client : connection_confirmation.other_clients) {
		add_peer(client.client_id, client.client_handle);
	}
}

void ClientPeerlist::add_peer(uint8_t id, std::string handle) {
	LOG_SCOPE_CLIENT;

	LocalNetPeer peer;
	peer.id = id;
	peer.handle = handle;
	add_peer(peer);
}

void ClientPeerlist::add_peer(LocalNetPeer& peer) {
	LOG_SCOPE_CLIENT;
	if (!connected) {
		Log::error("ClientPeerlist is not connected, cannot add peer");
		return;
	}

	// Check if peer already exists
	if (get_peer(peer.id).has_value()){
		Log::warn("Peer with ID " + std::to_string(peer.id) + " already exists, cannot add again");
		return;
	}
	peers.push_back(peer);
}

void ClientPeerlist::remove_peer(uint8_t id) {
	LOG_SCOPE_CLIENT;
	if (!connected) {
		Log::error("ClientPeerlist is not connected, cannot disconnect peer");
		return;
	}

	for (auto it = peers.begin(); it != peers.end(); ++it) {
		if (it->id == id) {
			peers.erase(it);
			return;
		}
	}
	Log::warn("Peer with ID " + std::to_string(id) + " not found, cannot disconnect");
}

void ClientPeerlist::remove_peer(std::string handle) {
	LOG_SCOPE_CLIENT;
	if (!connected) {
		Log::error("ClientPeerlist is not connected, cannot disconnect peer");
		return;
	}
	for (auto it = peers.begin(); it != peers.end(); ++it) {
		if (it->handle == handle) {
			peers.erase(it);
			return;
		}
	}
	Log::warn("Peer with handle " + handle + " not found, cannot disconnect");
}

void ClientPeerlist::remove_peer(LocalNetPeer& peer) {
	LOG_SCOPE_CLIENT;
	
	remove_peer(peer.id);
}

const std::optional<LocalNetPeer> ClientPeerlist::get_peer(uint8_t id) const {
	LOG_SCOPE_CLIENT;
	if (!connected) {
		Log::error("ClientPeerlist is not connected, cannot get peer");
	}

	for (const auto& p : peers) {
		if (p.id == id) {
			return p;
		}
	}
	// Not found
	return std::nullopt;
}

const std::optional<LocalNetPeer> ClientPeerlist::get_peer(std::string handle) const {
	LOG_SCOPE_CLIENT;
	if (!connected) {
		Log::error("ClientPeerlist is not connected, cannot get peer");
	}

	for (const auto& p : peers) {
		if (p.handle == handle) {
			return p;
		}
	}
	// Not found
	return std::nullopt;
}

const LocalNetPeer& ClientPeerlist::get_self() {
	LOG_SCOPE_CLIENT;
	if (!connected) {
		Log::error("ClientPeerlist is not connected, cannot get self");
	}
	return self;
}

NetPeer& ClientPeerlist::get_server() {
	LOG_SCOPE_CLIENT;
	if (!connected) {
		Log::error("ClientPeerlist is not connected, cannot get server");
	}
	return server;
}


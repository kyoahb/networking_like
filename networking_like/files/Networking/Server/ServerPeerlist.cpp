#include "ServerPeerlist.h"
#include "Utils/General/RandomUtils.h"

ServerPeerlist::ServerPeerlist() {
	self = LocalNetPeer(); // Initialize self as an empty LocalNetPeer
	self.handle = "Server_" + RandomUtils::random_string(4); // Set a default random handle for the server. This can be used to identify the server
	self.id = -1;
	self.is_host = true; // The server is always the host, along with one client
}

void ServerPeerlist::clear() {
	LOG_SCOPE_SERVER;
	peers.clear();
	reset_host_availability();
}

bool ServerPeerlist::check_if_host() {
	if (is_next_peer_host) {
		is_next_peer_host = false;
		is_host_set = true;
		return true; // This peer is the host
	}
	else {
		return false; // This peer is not the host
	}
}

void ServerPeerlist::add_peer(ENetPeer* peer, uint8_t id, std::string handle) {
	LOG_SCOPE_SERVER;
	NetPeer net_peer(peer, id, handle);

	net_peer.is_host = check_if_host();

	add_peer(net_peer);
}

void ServerPeerlist::add_peer(NetPeer peer) {
	LOG_SCOPE_SERVER;

	peer.is_host = check_if_host();

	peers.push_back(peer);
}

void ServerPeerlist::remove_peer(NetPeer& peer) {
	LOG_SCOPE_SERVER;
    for (auto it = peers.begin(); it != peers.end(); ++it) {  
        if (it->id == peer.id) {  
            peers.erase(it);  
            return;  
        }  
    }  
    Log::warn("Peer with ID " + std::to_string(peer.id) + " not found, cannot remove");  
}

void ServerPeerlist::remove_peer(uint8_t id) {
	LOG_SCOPE_SERVER;
	for (auto it = peers.begin(); it != peers.end(); ++it) {
		if (it->id == id) {
			peers.erase(it);
			return;
		}
	}
	Log::warn("Peer with ID " + std::to_string(id) + " not found, cannot remove");
}

void ServerPeerlist::remove_peer(std::string handle) {
	LOG_SCOPE_SERVER;
	for (auto it = peers.begin(); it != peers.end(); ++it) {
		if (it->handle == handle) {
			peers.erase(it);
			return;
		}
	}
	Log::warn("Peer with handle " + handle + " not found, cannot remove");
}

void ServerPeerlist::remove_peer(ENetPeer* peer) {
	LOG_SCOPE_SERVER;
	for (auto it = peers.begin(); it != peers.end(); ++it) {
		if (it->peer == peer) {
			peers.erase(it);
			return;
		}
	}
	enet_peer_reset(peer); // Just in case
	Log::warn("Peer with ENetPeer* " + std::to_string(reinterpret_cast<uintptr_t>(peer)) + " not found, cannot remove");
}

const std::optional<NetPeer> ServerPeerlist::get_peer(uint8_t id) const {
	LOG_SCOPE_SERVER;
	for (auto p : peers) {
		if (p.id == id) {
			return p;
		}
	}
	// Not found
	return std::nullopt;
}

const std::optional<NetPeer> ServerPeerlist::get_peer(std::string handle) const {
	LOG_SCOPE_SERVER;
	for (auto p : peers) {
		if (p.handle == handle) {
			return p;
		}
	}
	// Not found
	return std::nullopt;
}

const std::optional<NetPeer> ServerPeerlist::get_peer(ENetPeer* peer) const {
	LOG_SCOPE_SERVER;
	for (auto p : peers) {
		if (p.peer == peer) {
			return p;
		}
	}
	// Not found
	return std::nullopt;
}

std::string ServerPeerlist::get_polite_handle(ENetPeer* peer) const {
	LOG_SCOPE_SERVER;
	std::optional<NetPeer> found_peer = get_peer(peer);
	if (found_peer.has_value()) {
		return found_peer.value().handle + " ID_" + std::to_string(found_peer.value().id);
	}
	return "Unregistered_Peer No_" + std::to_string(peer->incomingPeerID);
}

std::vector<NetPeer> ServerPeerlist::get_peers() const {
	LOG_SCOPE_SERVER;

	return peers;
}

bool ServerPeerlist::is_peer_connected(ENetPeer* peer) const {
	LOG_SCOPE_SERVER;
	for (auto p : peers) {
		if (p.peer == peer) {
			return true;
		}
	}
	return false;
}

bool ServerPeerlist::is_peer_connected(uint8_t id) const {
	LOG_SCOPE_SERVER;
	for (auto p : peers) {
		if (p.id == id) {
			return true;
		}
	}
	return false;
}

bool ServerPeerlist::is_peer_connected(std::string handle) const {
	LOG_SCOPE_SERVER;
	for (auto p : peers) {
		if (p.handle == handle) {
			return true;
		}
	}
	return false;
}

const std::optional<NetPeer> ServerPeerlist::get_admin_peer() const {
	LOG_SCOPE_SERVER;

	for (auto p : peers) {
		if (p.is_host) {
			return p;
		}
	}
	return std::nullopt;
}

void ServerPeerlist::reset_host_availability() {
	LOG_SCOPE_SERVER;
	is_next_peer_host = true;
	is_host_set = false;
}

bool ServerPeerlist::is_peer_host(std::optional<NetPeer> peer) const {
	LOG_SCOPE_SERVER;
	if (!peer.has_value()) {
		return false; // No peer, no host
	}
	return peer->is_host;
}

const LocalNetPeer& ServerPeerlist::get_self() {
	LOG_SCOPE_SERVER;
	return self;
}
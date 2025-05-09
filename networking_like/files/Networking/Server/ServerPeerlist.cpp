#include "ServerPeerlist.h"

ServerPeerlist::ServerPeerlist() {

}

void ServerPeerlist::clear() {
	LOG_SCOPE_SERVER;
	peers.clear();
	self = LocalNetPeer();
}

void ServerPeerlist::add_peer(ENetPeer* peer, uint8_t id, std::string handle) {
	LOG_SCOPE_SERVER;
	NetPeer net_peer(peer, id, handle);
	add_peer(net_peer);
}

void ServerPeerlist::add_peer(NetPeer peer) {
	LOG_SCOPE_SERVER;
	peers[peer.id] = peer;
}

void ServerPeerlist::remove_peer(NetPeer& peer) {
	LOG_SCOPE_SERVER;
	if (peers.contains(peer.id)) {
		peers.erase(peer.id);
		return;
	}
	Log::warn("Peer with ID " + std::to_string(peer.id) + " not found, cannot remove");
}

void ServerPeerlist::remove_peer(uint8_t id) {
	LOG_SCOPE_SERVER;
	if (peers.contains(id)) {
		peers.erase(id);
		return;
	}
	Log::warn("Peer with ID " + std::to_string(id) + " not found, cannot remove");
}

void ServerPeerlist::remove_peer(std::string handle) {
	LOG_SCOPE_SERVER;
	for (auto it = peers.begin(); it != peers.end(); ++it) {
		if (it->second.handle == handle) {
			peers.erase(it);
			return;
		}
	}
	Log::warn("Peer with handle " + handle + " not found, cannot remove");
}

void ServerPeerlist::remove_peer(ENetPeer* peer) {
	LOG_SCOPE_SERVER;
	for (auto it = peers.begin(); it != peers.end(); ++it) {
		if (it->second.peer == peer) {
			peers.erase(it);
			return;
		}
	}
	enet_peer_reset(peer); // Just in case
	Log::warn("Peer with ENetPeer* " + std::to_string(reinterpret_cast<uintptr_t>(peer)) + " not found, cannot remove");
}

const std::optional<NetPeer> ServerPeerlist::get_peer(uint8_t id) const {
	LOG_SCOPE_SERVER;
	if (peers.contains(id)) {
		return peers.at(id);
	}
	// Not found
	return std::nullopt;
}

const std::optional<NetPeer> ServerPeerlist::get_peer(std::string handle) const {
	LOG_SCOPE_SERVER;
	for (const auto& p : peers) {
		if (p.second.handle == handle) {
			return p.second;
		}
	}
	// Not found
	return std::nullopt;
}

const std::optional<NetPeer> ServerPeerlist::get_peer(ENetPeer* peer) const {
	LOG_SCOPE_SERVER;
	for (const auto& p : peers) {
		if (p.second.peer == peer) {
			return p.second;
		}
	}
	// Not found
	return std::nullopt;
}

const LocalNetPeer& ServerPeerlist::get_self() {
	LOG_SCOPE_SERVER;
	return self;
}

const std::string& ServerPeerlist::get_polite_handle(ENetPeer* peer) const {
	LOG_SCOPE_SERVER;
	std::optional<NetPeer> found_peer = get_peer(peer);
	if (found_peer.has_value()) {
		return found_peer.value().handle + " ID_" + std::to_string(peer->incomingPeerID);
	}
	return "Unregistered_Peer ID_" + std::to_string(peer->incomingPeerID);
}

std::vector<NetPeer> ServerPeerlist::get_peers() const {
	LOG_SCOPE_SERVER;
	std::vector<NetPeer> peer_list;
	for (const auto& p : peers) {
		peer_list.push_back(p.second);
	}
	return peer_list;
}
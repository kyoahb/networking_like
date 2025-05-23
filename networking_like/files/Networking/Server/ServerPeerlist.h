#pragma once

#include "Utils/Imports/common.h"
#include "Networking/Shared/NetPeer.h"

class ServerPeerlist {
public:
	std::vector<NetPeer> peers; // List of connected clients

	ServerPeerlist();

	void clear(); // Clear the peer list, used when server stops

	void add_peer(ENetPeer* peer, uint8_t id, std::string handle); // Add a peer to the list
	void add_peer(NetPeer peer); // Add a peer to the list

	void remove_peer(NetPeer& peer); // Remove a peer from the list
	void remove_peer(uint8_t id); // Remove a peer from the list
	void remove_peer(std::string handle); // Remove a peer from the list
	void remove_peer(ENetPeer* peer); // Remove a peer from the list

	const std::optional<NetPeer> get_peer(uint8_t id) const; // Get a peer by ID
	const std::optional<NetPeer> get_peer(std::string handle) const; // Get a peer by handle
	const std::optional<NetPeer> get_peer(ENetPeer* peer) const; // Get a peer by ENetPeer*

	bool is_peer_connected(ENetPeer* peer) const; // Check if a peer is connected by ENetPeer*
	bool is_peer_connected(uint8_t id) const; // Check if a peer is connected by ID
	bool is_peer_connected(std::string handle) const; // Check if a peer is connected by handle

	void reset_host_availability(); // Reset the host availability, used when server stops
	const std::optional<NetPeer> get_admin_peer() const; // Get the admin peer (the first peer in the list)

	std::string get_polite_handle(ENetPeer* peer) const; // If handle exists, return [handle] ID_[id]. Else, return Unregistered_Peer ID_[id]

	const LocalNetPeer& get_self(); // Get the server self

	std::vector<NetPeer> get_peers() const; // Get all peers

private:
	bool is_next_peer_host = true;
	bool is_host_set = false; // Is the host set?
	// Note: host is ONLY the first peer that joins
	// If the first peer joins then leaves, there is no host and cannot be a host again
	// Unless server is reset

};
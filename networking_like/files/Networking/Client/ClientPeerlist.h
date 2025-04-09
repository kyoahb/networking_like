#pragma once
#include "Utils/Imports/common.h"
#include "Networking/Shared/NetPeer.h"

class ClientPeerlist {
public:
	bool connected = false;
	LocalNetPeer self; // Local peer (client)
	std::vector<LocalNetPeer> peers; // Local, as client should not be able to access ENetPeer* for other clients.
	NetPeer server; // Connected server peer

	ClientPeerlist() = default;

	void add_peer(uint8_t id, std::string handle);
	void add_peer(LocalNetPeer& peer);

	void disconnect_peer(uint8_t id);
	void disconnect_peer(LocalNetPeer& peer);
	void disconnect_peer(std::string handle);
};
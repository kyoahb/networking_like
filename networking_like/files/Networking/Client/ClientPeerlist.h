#pragma once
#include "Utils/Imports/common.h"
#include "Networking/Shared/NetPeer.h"

#include "Networking/Packets/Data/ClientConnect.h"

class ClientPeerlist {
public:

	ClientPeerlist();

	void clear(); // Clear the peer list, used when disconnecting from the server

	void connect(ENetPeer* server_peer);
	void setup_by_confirmation(const ClientConnectConfirm& connection_confirmation);

	void add_peer(uint8_t id, std::string handle);
	void add_peer(LocalNetPeer& peer);

	void remove_peer(uint8_t id);
	void remove_peer(LocalNetPeer& peer);
	void remove_peer(std::string handle);

	const std::optional<LocalNetPeer> get_peer(uint8_t id) const;
	const std::optional<LocalNetPeer> get_peer(std::string handle) const;

	std::string get_polite_handle(uint8_t id) const;

	const LocalNetPeer& get_self();
	NetPeer& get_server();

	bool is_connected();
	std::vector<LocalNetPeer> get_peers() const;

private:
	bool connected = false;
	LocalNetPeer self; // Local peer (client)
	std::vector<LocalNetPeer> peers; // Local, as client should not be able to access ENetPeer* for other clients.
	NetPeer server; // Connected server peer
};
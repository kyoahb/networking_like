#pragma once
#include "Networking/Packets/PacketData.h"
#include "Networking/Packets/Data/ClientConnectRelay.h"

// Server -> Client
// Subtype 3 (CLIENT_CONNECT_CONFIRM)
class ClientConnectConfirm : public PacketData {
public:
	std::string server_preferred_handle = ""; // The server's preferred handle
	
	std::string client_decided_handle = ""; // The client's decided handle (may be different if server decides)
	uint8_t client_id = 0; // The client's ID (assigned by the server)

	std::vector<ClientConnectRelay> other_clients; // Other clients that are connected to the server

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(server_preferred_handle, client_decided_handle, client_id, other_clients);
	}

};
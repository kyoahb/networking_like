#pragma once
#include "Networking/Packets/PacketData.h"

// Server -> Client
// Subtype 2 (CLIENT_CONNECT_RELAY)
class ClientConnectRelay : public PacketData {
public:
	uint8_t client_id = 0; // ID of the client that connected
	std::string client_handle = ""; // Handle of the client that connected


	template <class Archive>
	void serialize(Archive& archive) {
		archive(client_id, client_handle);
	}
};
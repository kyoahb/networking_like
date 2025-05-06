#pragma once
#include "Networking/Packets/PacketData.h"

// Server -> Client
// Subtype 4 (CLIENT_DISCONNECT_RELAY)
class ClientDisconnectRelay : public PacketData {
public:
	uint8_t client_id = 0; // ID of the client that connected

	template <class Archive>
	void serialize(Archive& archive) {
		archive(client_id);
	}
};
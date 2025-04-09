#pragma once
#include "Networking/Packets/PacketData.h"

// Client -> Server
// Subtype 1 (CLIENT_CONNECT_BEGIN)
class ClientConnectBegin : public PacketData {
public:
	std::string client_preferred_handle = "";

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(client_preferred_handle);
	}
};
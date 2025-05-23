#pragma once
#include "Networking/Packets/PacketData.h"
#include "Networking/Shared/FutureResults/DisconnectResult.h"

enum ClientDisconnectType : uint8_t {
	CLIENT_DISCONNECT_ANY = 255,
	CLIENT_DISCONNECT_BEGIN = 1,
	CLIENT_DISCONNECT_RELAY = 2,
	CLIENT_DISCONNECT_CONFIRM = 3,
};

// Direction can be Any: (C->S) or (S->C)
// Subtype 1 (CLIENT_DISCONNECT_BEGIN)
class ClientDisconnectBegin : public PacketData {
public:
	DisconnectResultType type = DisconnectResultType::UNKNOWN; // Type of disconnect result
	DisconnectResultReason reason = DisconnectResultReason::UNKNOWN; // Reason for the disconnect result
	std::string message = ""; // Message associated with the disconnect result

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(type, reason, message);
	}
};

// Direction is ONLY Server -> Client
// Subtype 2 (CLIENT_DISCONNECT_RELAY)
class ClientDisconnectRelay : public PacketData {
public:
	uint8_t client_id = 0; // ID of the client that has disconnected

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(client_id);
	}
};

// Direction can be Any: (C->S) or (S->C)
// Subtype 3 (CLIENT_DISCONNECT_CONFIRM)
class ClientDisconnectConfirm : public PacketData {
public:
	// No data.. maybe add some later?
	std::string message = "";
	template <typename Archive>
	void serialize(Archive& archive) {
		archive(message);
	}
};
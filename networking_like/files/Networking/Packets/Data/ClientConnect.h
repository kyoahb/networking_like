#pragma once
#include "Utils/Imports/common.h"
#include "Networking/Packets/PacketData.h"

enum ClientConnectType : uint8_t {
	CLIENT_CONNECT_ANY = 255,
	CLIENT_CONNECT_BEGIN = 1,
	CLIENT_CONNECT_RELAY = 2,
	CLIENT_CONNECT_CONFIRM = 3,
};

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


// Server -> Client
// Subtype 2 (CLIENT_CONNECT_RELAY)
class ClientConnectRelay : public PacketData {
public:
	bool is_host = false; // Is the connected client the server host?
	uint8_t client_id = 0; // ID of the client that connected
	std::string client_handle = ""; // Handle of the client that connected


	template <typename Archive>
	void serialize(Archive& archive) {
		archive(is_host, client_id, client_handle);
	}
};

// Server -> Client
// Subtype 3 (CLIENT_CONNECT_CONFIRM)
class ClientConnectConfirm : public PacketData {
public:
	std::string server_preferred_handle = ""; // The server's preferred handle
	bool is_host = false; // Has the server recognised this client as the host?
	std::string client_decided_handle = ""; // The client's decided handle (may be different if server decides)
	uint8_t client_id = 0; // The client's ID (assigned by the server)

	std::vector<ClientConnectRelay> other_clients; // Other clients that are connected to the server

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(server_preferred_handle, is_host, client_decided_handle, client_id, other_clients);
	}

};
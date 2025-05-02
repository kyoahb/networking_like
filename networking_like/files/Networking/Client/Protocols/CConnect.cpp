#include "CConnect.h"
#include "Networking/Client/Client.h"


CConnect::CConnect(std::shared_ptr<Client> _client) : CProtocol(_client, "CConnect") {
	LOG_SCOPE_CLIENT_PROTOCOL;

}

void CConnect::start() {};
void CConnect::stop() {};
void CConnect::update() {};
void CConnect::destroy() {};
void CConnect::packet_event(const ENetEvent& event) {
	LOG_SCOPE_CLIENT_PROTOCOL;
	
}


ConnectResult CConnect::connect(const std::string& ip, uint16_t port, const std::string& preferred_handle) {
	LOG_SCOPE_CLIENT_PROTOCOL;

	uint64_t start_time = TimeUtils::get_current_time_millis();

	if (!client) {
		Log::asserts(false, "Client is null in CConnect::connect()");
	}

	// Check if the client is already connected
	if (client->is_connected()) {
		return ConnectResult{ ConnectResultType::FAILURE, "Client is already connected", 0 };
	}

	// Initialise connection
	ENetPeer* server_peer = begin_connection(ip, port, preferred_handle);
	if (server_peer == nullptr) {
		Log::error("Failed to connect to server");
		return ConnectResult{ ConnectResultType::FAILURE, "No available peers", TimeUtils::get_current_time_millis() - start_time };
	}

	// Check that connection was successful
	if (!wait_for_connection_establishment(server_peer)) {
		Log::error("Connection to server failed");
		enet_peer_reset(server_peer); // client->disconnect(); 
		return ConnectResult{ ConnectResultType::FAILURE, "Connection to server failed", TimeUtils::get_current_time_millis() - start_time };
	}

	// We are certain initial connection is successful by now.
	(client->peers).connect(server_peer); // Setup server in peerlist

	send_connection_begin_packet(preferred_handle); // Send connection begin packet

	// Wait for CLIENT_CONNECT_CONFIRM packet
	if (!wait_for_connection_confirmation()) {
		Log::error("Connection confirmation timed out");
		client->disconnect().wait();
		return ConnectResult{ ConnectResultType::FAILURE, "Connection confirmation timed out", TimeUtils::get_current_time_millis() - start_time };
	}

	// Connection confirmation received
	Log::trace("Connection complete, start()ing");
	return ConnectResult{ ConnectResultType::SUCCESS, "Connected to server", TimeUtils::get_current_time_millis() - start_time };
}

ENetPeer* CConnect::begin_connection(const std::string& ip, uint16_t port, const std::string& preferred_handle) {
	LOG_SCOPE_CLIENT_PROTOCOL;

	// Initialise connection
	ENetAddress* address = &(client->address);
	ENetHost* host = (client->host);

	enet_address_set_host(address, ip.c_str());
	address->port = port;

	ENetPeer* server_peer = enet_host_connect(host, address, 2, 0);

	return server_peer; // Check if valid will be done in connect()
}

bool CConnect::wait_for_connection_establishment(ENetPeer* server_peer) {
	LOG_SCOPE_CLIENT_PROTOCOL;

	const int TIMEOUT_MS = 5000;
	const int UPDATE_INTERVAL_MS = 1000;
	unsigned int elapsed_ms = 0;
	ENetEvent event;
	while (elapsed_ms < TIMEOUT_MS) {
		if (enet_host_service(client->host, &event, UPDATE_INTERVAL_MS) > 0) {
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				Packet received_packet(event.packet);
				if (received_packet.header.type == PacketType::CLIENT_CONNECT && received_packet.header.subtype == ClientConnectType::CLIENT_CONNECT_CONFIRM) {
					ClientConnectConfirm connection_confirmation = SerializationUtils::deserialize<ClientConnectConfirm>(received_packet.data, received_packet.header.size);
					client->peers.setup_by_confirmation(connection_confirmation);

					Log::trace("CLIENT_CONNECT:CONFIRM received. Server-given handle for US: " + connection_confirmation.client_decided_handle);

					return true;
				}
			}
		}
		elapsed_ms += UPDATE_INTERVAL_MS;
		Log::trace("Waiting for connection confirmation... " + std::to_string(elapsed_ms) + "ms elapsed");
	}
	return false;
}

void CConnect::send_connection_begin_packet(const std::string& preferred_handle) {
	LOG_SCOPE_CLIENT;

	ClientConnectBegin connect_begin;
	connect_begin.client_preferred_handle = preferred_handle;

	std::string serialised_data = SerializationUtils::serialize<ClientConnectBegin>(connect_begin);
	Packet packet(PacketType::CLIENT_CONNECT, PacketDirection::CLIENT_TO_SERVER, ClientConnectType::CLIENT_CONNECT_BEGIN, serialised_data.data(), serialised_data.size(), true);

	client->send_packet(packet);
}

bool CConnect::wait_for_connection_confirmation() {
	LOG_SCOPE_CLIENT;
	const int TIMEOUT_MS = 5000;
	const int UPDATE_INTERVAL_MS = 1000;
	unsigned int elapsed_ms = 0;
	ENetEvent event;
	while (elapsed_ms < TIMEOUT_MS) {
		if (enet_host_service(client->host, &event, UPDATE_INTERVAL_MS) > 0) {
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				Packet received_packet(event.packet);
				if (received_packet.header.type == PacketType::CLIENT_CONNECT && received_packet.header.subtype == ClientConnectType::CLIENT_CONNECT_CONFIRM) {
					ClientConnectConfirm connection_confirmation = SerializationUtils::deserialize<ClientConnectConfirm>(received_packet.data, received_packet.header.size);
					client->peers.setup_by_confirmation(connection_confirmation);

					Log::trace("CLIENT_CONNECT:CONFIRM received. Server-given handle for US: " + connection_confirmation.client_decided_handle);

					return true;
				}
			}
		}
		elapsed_ms += UPDATE_INTERVAL_MS;
		Log::trace("Waiting for connection confirmation... " + std::to_string(elapsed_ms) + "ms elapsed");
	}
	return false;
}
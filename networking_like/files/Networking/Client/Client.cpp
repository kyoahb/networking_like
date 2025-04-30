#include "Client.h"

Client::Client() : NetworkUser(), peers(ClientPeerlist()) {
	LOG_SCOPE_CLIENT;

	host = enet_host_create(nullptr, 1, 2, 0, 0);
	if (host == nullptr) {
		Log::error("Failed to create ENet host");
		return;
	}
}

Client::~Client() {
	LOG_SCOPE_CLIENT;
	if (is_connected()) {
		Log::warn("Client is still connected, disconnecting...");
		disconnect().wait();
	}
	if (active) {
		stop();
	}
}

bool Client::is_connected() {
	LOG_SCOPE_CLIENT;
	return peers.is_connected();
}

// CONNECTION LOGIC

std::future<ConnectResult> Client::connect(const std::string& ip, uint16_t port, const std::string& preferred_handle) {
	LOG_SCOPE_CLIENT;
	Log::trace("Connecting to server at " + ip + ":" + std::to_string(port) + " with preferred handle: " + preferred_handle);

	return std::async(std::launch::async, [this, ip, port, preferred_handle]() {
		return connect_thread(ip, port, preferred_handle);
	});
}

bool Client::wait_for_connection_establishment(ENetPeer* server_peer) {
	LOG_SCOPE_CLIENT;
	if (server_peer == nullptr) {
		Log::error("Server peer is nullptr, cannot wait for connection establishment");
		return false;
	}

	const int TIMEOUT_MS = 5000;
	const int UPDATE_INTERVAL_MS = 1000;
	unsigned int elapsed_ms = 0;
	ENetEvent event;

	while (elapsed_ms < TIMEOUT_MS) {
		if (enet_host_service(host, &event, UPDATE_INTERVAL_MS) > 0) {
			if (event.type == ENET_EVENT_TYPE_CONNECT) {
				Log::trace("Connection successful after " + std::to_string(elapsed_ms) + "ms. Connected to server");
				return true;
			}
		}
		elapsed_ms += UPDATE_INTERVAL_MS;
		Log::trace("Waiting for connection... " + std::to_string(elapsed_ms) + "ms elapsed");
	}
	return false;
}

void Client::send_connection_begin_packet(const std::string& preferred_handle) {
	LOG_SCOPE_CLIENT;

	ClientConnectBegin connect_begin;
	connect_begin.client_preferred_handle = preferred_handle;
	
	std::string serialised_data = SerializationUtils::serialize<ClientConnectBegin>(connect_begin);
	Packet packet(PacketType::CLIENT_CONNECT, PacketDirection::CLIENT_TO_SERVER, ClientConnectType::CLIENT_CONNECT_BEGIN, serialised_data.data(), serialised_data.size(), true);
	
	send_packet(packet);
}

bool Client::wait_for_connection_confirmation() {
	LOG_SCOPE_CLIENT;
	const int TIMEOUT_MS = 5000;
	const int UPDATE_INTERVAL_MS = 1000;
	unsigned int elapsed_ms = 0;
	ENetEvent event;
	while (elapsed_ms < TIMEOUT_MS) {
		if (enet_host_service(host, &event, UPDATE_INTERVAL_MS) > 0) {
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				Packet received_packet(event.packet);
				if (received_packet.header.type == PacketType::CLIENT_CONNECT && received_packet.header.subtype == ClientConnectType::CLIENT_CONNECT_CONFIRM) {
					ClientConnectConfirm connection_confirmation = SerializationUtils::deserialize<ClientConnectConfirm>(received_packet.data, received_packet.header.size);
					peers.setup_by_confirmation(connection_confirmation);

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

ConnectResult Client::connect_thread(const std::string& ip, uint16_t port, const std::string& preferred_handle) {
	LOG_SCOPE_CLIENT;

	uint64_t start_time = TimeUtils::get_current_time_millis();

	if (is_connected()) {
		Log::warn("Client is already connected, cannot connect again");
		
		return ConnectResult{ ConnectResultType::FAILURE, "Client is already connected", 0 };
	}

	// Initialise connection
	enet_address_set_host(&address, ip.c_str());
	address.port = port;

	ENetPeer* server_peer = enet_host_connect(host, &address, 2, 0);
	if (server_peer == nullptr) {
		Log::error("Failed to connect to server");
		return ConnectResult{ ConnectResultType::FAILURE, "No available peers", TimeUtils::get_current_time_millis()-start_time };
	}

	// Check that connection was successful
	if (!wait_for_connection_establishment(server_peer)) {
		Log::error("Connection to server failed");
		enet_peer_reset(server_peer);
		return ConnectResult{ ConnectResultType::FAILURE, "Connection to server failed", TimeUtils::get_current_time_millis() - start_time };
	}

	// We are certain initial connection is successful by now.
	peers.connect(server_peer); // Setup server in peerlist

	send_connection_begin_packet(preferred_handle); // Send connection begin packet

	// Wait for CLIENT_CONNECT_CONFIRM packet
	if (!wait_for_connection_confirmation()) {
		Log::error("Connection confirmation timed out");
		disconnect().wait();
		return ConnectResult{ ConnectResultType::FAILURE, "Connection confirmation timed out", TimeUtils::get_current_time_millis() - start_time };
	}

	// Connection confirmation received
	Log::trace("Connection complete, start()ing");
	start();
	return ConnectResult{ ConnectResultType::SUCCESS, "Connected to server", TimeUtils::get_current_time_millis() - start_time };
}



// DISCONNECT LOGIC

std::future<DisconnectResult> Client::disconnect() {
	LOG_SCOPE_CLIENT;
	Log::trace("Disconnecting from server... calling stop()");
	if (active) {
		stop();
	}

	return std::async(std::launch::async, [this]() {
		return disconnect_thread(DisconnectResultReason::USER_REQUESTED);
	});
}

DisconnectResult Client::disconnect_thread(DisconnectResultReason reason) {
	LOG_SCOPE_CLIENT;

	DisconnectResult result;
	result.type = DisconnectResultType::UNKNOWN;
	result.reason = DisconnectResultReason::UNKNOWN;

	if (!is_connected()) {
		Log::warn("Client is not connected, cannot disconnect");
		result.type = DisconnectResultType::FAILED;
		result.reason = DisconnectResultReason::USER_REQUESTED;
		return result;
	}

	uint64_t start_time = TimeUtils::get_current_time_millis();

	// Send disconnect packet
	enet_peer_disconnect(peers.get_server().peer, 0);

	// Wait for disconnect confirmation
	const int TIMEOUT_MS = 5000;
	const int UPDATE_INTERVAL_MS = 1000;
	unsigned int elapsed_ms = 0;
	ENetEvent event;

	bool confirmed = false;
	while ((elapsed_ms < TIMEOUT_MS) && !confirmed) {
		if (enet_host_service(host, &event, UPDATE_INTERVAL_MS) > 0) {
			switch (event.type) {
			case ENET_EVENT_TYPE_DISCONNECT:
				Log::trace("Disconnected from server");
				confirmed = true;
				break;
			default:
				enet_packet_destroy(event.packet);
				break;
			}
		}
		elapsed_ms += UPDATE_INTERVAL_MS;
		Log::trace("Waiting for disconnect confirmation... " + std::to_string(elapsed_ms) + "ms elapsed");
	}

	if (elapsed_ms >= TIMEOUT_MS) {
		Log::error("Disconnect confirmation timed out");
		result.type = DisconnectResultType::FORCED;
		enet_peer_reset(peers.get_server().peer);
	}
	else {
		Log::trace("Disconnect confirmation received");
		result.type = DisconnectResultType::SUCCESS;
	}

	peers.clear();
	
	result.time_taken = TimeUtils::get_current_time_millis() - start_time;
	return result;
}

// PACKET SENDING

bool Client::send_packet(const Packet& packet) {
	LOG_SCOPE_CLIENT;
	Log::trace("Sending packet " + PacketHelper::types_to_string(packet) + " to server");
	
	if (!is_connected()) {
		Log::error("Client is not connected, cannot send packet");
		return false;
	}
	
	if (packet.header.direction != PacketDirection::CLIENT_TO_SERVER) {
		Log::error("Packet direction is not CLIENT_TO_SERVER, cannot send packet");
		return false;
	}
	

	return NetworkUser::send_packet(packet, peers.get_server());
}


// UPDATE LOOP

void Client::start() {
	LOG_SCOPE_CLIENT;

	if (!is_connected()) {
		Log::warn("Client is not connected, cannot start update loop as it is pointless");
		return;
	}

	if (update_thread_id == std::this_thread::get_id()) {
		Log::warn("Client update loop is already running in this thread, cannot start again");
		return;
	}

	if (active) {
		Log::warn("Client update loop is already running, cannot start again");
		return;
	}

	NetworkUser::start();
}

void Client::stop() {
	LOG_SCOPE_CLIENT;

	if (!active) {
		Log::warn("Client update loop is not running, cannot stop");
		return;
	}

	NetworkUser::stop();
}

void Client::update() {
	LOG_SCOPE_CLIENT;
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_RECEIVE:
			receive_event(event);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			disconnect_event(event);
		
			return; // Stop update loop if disconnected
		default:
			break;
		}
	}
}

// EVENTS

void Client::disconnect_event(const ENetEvent& event) {
	LOG_SCOPE_CLIENT;
	Log::trace("Forceful disconnect event received, stopping");
	
	enet_packet_destroy(event.packet);
	peers.clear();
	enet_peer_reset(event.peer);

	stop();
}

void Client::receive_event(const ENetEvent& event) {
	LOG_SCOPE_CLIENT;
	
	/*
	Packet p(event.packet);
	Log::trace("Received packet: " + PacketHelper::types_to_string(p) + " from " + peers.get_server().handle);
	*/
	
}
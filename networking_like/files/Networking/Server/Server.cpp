#include "Server.h"

Server::Server(const std::string& _address, int _port) : NetworkUser(), address_str(_address), port(_port) {
	LOG_SCOPE_SERVER;
	Log::trace("Server constructor called with address: " + address_str + " and port: " + std::to_string(port));
	
	// Initialize the server
	if (enet_address_set_host(&address, address_str.c_str()) != 0) {
		Log::error("Failed to set host address");
	};

	//address.host = ENET_HOST_ANY;
	address.port = port;

	host = enet_host_create(&address, MAX_CLIENTS, MAX_CHANNELS, BANDWIDTH_LIMIT, TIMEOUT_LIMIT);
	Log::asserts(host != NULL, "Failed to create ENet host");

	Log::trace("Server initialized!");
}

void Server::add_protocol(std::shared_ptr<SProtocol> protocol) {
	LOG_SCOPE_SERVER;

	protocols.push_back(protocol);
}

void Server::initialize_protocols() {
	LOG_SCOPE_SERVER;
	Log::trace("Initializing protocols");
	
	// Built in protocols
	std::shared_ptr<SDisconnect> disconnect = std::make_shared<SDisconnect>(shared_from_this());
	disconnect_protocol = disconnect;
	add_protocol(disconnect);

	std::shared_ptr<SConnect> connect = std::make_shared<SConnect>(shared_from_this());
	add_protocol(connect);
}

void Server::start_protocols() {
	LOG_SCOPE_SERVER;
	Log::trace("Starting protocols");
	for (auto& protocol : protocols) {
		protocol->start();
	}
}

void Server::stop_protocols() {
	LOG_SCOPE_SERVER;
	Log::trace("Stopping protocols");
	for (auto& protocol : protocols) {
		protocol->stop();
	}
}

void Server::update_protocols() {
	LOG_SCOPE_SERVER;
	for (auto& protocol : protocols) {
		protocol->update();
	}
}

void Server::destroy_protocols() {
	LOG_SCOPE_SERVER;
	Log::trace("Destroying protocols");
	for (auto& protocol : protocols) {
		protocol->destroy();
	}
	protocols.clear();
}

void Server::broadcast_packet(const Packet& packet, std::vector<NetPeer> excluding) {
	LOG_SCOPE_SERVER;
	Log::trace("Broadcasting packet " + PacketHelper::types_to_string(packet) + " to all peers");
	for (auto& peer : peers.peers) {
		if (peer.peer != nullptr && peer.peer->state != ENET_PEER_STATE_DISCONNECTED) {
			if (std::find(excluding.begin(), excluding.end(), peer) == excluding.end()) {
				send_packet(packet, peer);
			}
		}
	}
}

void Server::dispatch_event_to_protocols(const ENetEvent& event) {
	std::optional<NetPeer> peer = std::nullopt;

	if (event.peer != nullptr) {
		peer = peers.get_peer(event.peer); // Nullptr if peer not in list
	}

	for (auto& protocol : protocols) {
		protocol->packet_event(event, peer);
	}
}

Server::~Server() {
	LOG_SCOPE_SERVER;
	Log::trace("Server destructor called");

	destroy_protocols();
	if (active) {
		stop().wait();
	}

	if (host != nullptr) {
		enet_host_destroy(host);
		host = nullptr;
	}

	Log::trace("Server destroyed");
}

void Server::start() {
	LOG_SCOPE_SERVER;
	
	initialize_protocols();

	if (active) {
		Log::warn("Server is already active, cannot start");
		return;
	}

	active = true;
	start_protocols();
	update_future = std::async(std::launch::async, &Server::update_loop, this);
	Log::trace("Server started update loop.");
}

std::future<ShutdownResult> Server::stop() {
	LOG_SCOPE_SERVER;

	if (!active) {
		Log::warn("Server is not active, cannot stop");
		return std::async(std::launch::async, []() { 
			return ShutdownResult(ShutdownResultType::FAILURE, 0, "Server is already inactive.", 0, 0);
		});
	}

	active = false;
	stop_protocols();
	if (update_future.valid() && std::this_thread::get_id() != update_thread_id) {
		update_future.get(); // Wait for the update loop to finish
	}
	else {
		Log::warn("Update loop is in the same thread, cannot stop()");
	}
	return std::async(std::launch::async, &Server::stop_thread, this);
}

ShutdownResult Server::stop_thread() {
	LOG_SCOPE_SERVER;
	Log::trace("Server stop thread started");

	// Check if the server is active
	if (!active) {
		Log::warn("Server is not active, cannot stop");
		return ShutdownResult(ShutdownResultType::FAILURE, 0, "Server is already inactive.", 0, 0);
	}
	
	// Send disconnect packets to all peers, and add pending disconnects
	uint64_t start_time = TimeUtils::get_current_time_millis();
	uint8_t peers_graceful = 0;
	uint8_t peers_forced = 0;
	std::vector<std::future<DisconnectResult>> disconnect_futures;

	// Send disconnect packets to all peers
	for (auto& peer : peers.peers) {
		if (peer.peer != nullptr && peer.peer->state != ENET_PEER_STATE_DISCONNECTED) {
			// Add future to track disconnect result
			disconnect_futures.push_back(disconnect_peer(peer, DisconnectResultReason::SERVER_STOPPING));
		}
	}

	// Wait for all disconnects to complete
	for (auto& future : disconnect_futures) {
		DisconnectResult result = future.get();
		if (result.type == DisconnectResultType::SUCCESS) {
			peers_graceful++;
		}
		else if (result.type == DisconnectResultType::FORCED) {
			peers_forced++;
		}
	}

	// Clear the peer list
	peers.clear();

	// Calculate time taken
	uint64_t time_taken = TimeUtils::get_current_time_millis() - start_time;

	// Create and return the result
	ShutdownResult result;
	result.type = ShutdownResultType::SUCCESS;
	result.time_taken = time_taken;
	result.message = "Server stopped successfully";
	result.peers_graceful = peers_graceful;
	result.peers_forced = peers_forced;

	Log::trace("Server stopped successfully. Graceful disconnects: " + std::to_string(peers_graceful) +
		", Forced disconnects: " + std::to_string(peers_forced) +
		", Time taken: " + std::to_string(time_taken) + "ms");

	return result;
}

std::future<DisconnectResult> Server::disconnect_peer(NetPeer& peer, DisconnectResultReason reason) {
	LOG_SCOPE_SERVER;
	Log::trace("Disconnecting peer: " + peer.handle);

	// Verify we have the disconnect protocol
	if (!disconnect_protocol) {
		Log::error("Disconnect protocol not initialized!");
		return std::async(std::launch::async, [reason]() {
			return DisconnectResult(DisconnectResultType::FAILED, reason, "Disconnect protocol not initialized", 0);
			});
	}

	// Use the protocol to disconnect the peer
	return std::async(std::launch::async, [this, &peer, reason]() {
		DisconnectResult result = disconnect_protocol->disconnect_peer(peer, reason);

		// We still need to remove the peer from the server's list after disconnection
		if (result.type == DisconnectResultType::SUCCESS || result.type == DisconnectResultType::FORCED) {
			peers.remove_peer(peer);
		}

		return result;
		});
}

void Server::update() {
	LOG_SCOPE_SERVER;
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		dispatch_event_to_protocols(event);
		switch (event.type) {
		case ENET_EVENT_TYPE_CONNECT:
			connect_event(event);
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			receive_event(event);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			disconnect_event(event);
			break;
		default:
			enet_packet_destroy(event.packet);
			break;
		}

	}

	// Check timeouts
	enet_host_flush(host);

	update_protocols();
}
// Events

void Server::disconnect_event(const ENetEvent& event) {
	LOG_SCOPE_SERVER;
}

void Server::receive_event(const ENetEvent& event) {
	LOG_SCOPE_SERVER;
	/*
	Packet p(event.packet);
	std::string peer_handle = "No_Handle";

	std::optional<NetPeer> peer = peers.get_peer(event.peer);
	if (peer.has_value()) {
		peer_handle = peer.value().handle;
	}

	Log::trace("Received packet: " + PacketHelper::types_to_string(p) + " from " + peer_handle);
	*/
}

void Server::connect_event(const ENetEvent& event) {
	LOG_SCOPE_SERVER;
}

bool Server::send_packet(const Packet& packet, const NetPeer& peer) {
	LOG_SCOPE_SERVER;

	if (packet.header.direction != PacketDirection::SERVER_TO_CLIENT) {
		Log::error("Invalid packet header direction, must be S->C");
	}

	Log::trace("Sending packet " + PacketHelper::types_to_string(packet) + " to peer " + peer.handle);

	return NetworkUser::send_packet(packet, peer);
}
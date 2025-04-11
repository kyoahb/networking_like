#include "Server.h"

Server::Server(const std::string& _address, int _port) : address_str(_address), port(_port) {
	LOG_SCOPE_SERVER;
	Log::trace("Server constructor called with address: " + address_str + " and port: " + std::to_string(port));
	
	// Initialize the server
	enet_address_set_host(&address, address_str.c_str());
	address.port = port;

	host = enet_host_create(&address, MAX_CLIENTS, MAX_CHANNELS, BANDWIDTH_LIMIT, TIMEOUT_LIMIT);
	Log::asserts(host != nullptr, "Failed to create ENet host");


	/* Protocols */
	// Initialize built-in protocols
	std::shared_ptr<SDisconnect> disconnect = std::make_shared<SDisconnect>();
	disconnect_protocol = disconnect;
	protocols.push_back(disconnect);

	initialize_protocols();



	Log::trace("Server initialized!");
}

void Server::initialize_protocols() {
	LOG_SCOPE_SERVER;
	Log::trace("Initializing protocols");

	for (auto& protocol : protocols) {
		protocol->init();
	}
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

void Server::dispatch_event_to_protocols(const ENetEvent& event) {
	std::optional<NetPeer> peer = std::nullopt;

	if (event.peer != nullptr) {
		peer = peers.get_peer(event.peer);
	}

	for (auto& protocol : protocols) {
		protocol->receive_event(event, peer);
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

	NetworkUser::receive_event(event);
}

void Server::connect_event(const ENetEvent& event) {
	LOG_SCOPE_SERVER;
	// Check if the peer is valid
	if (event.peer == nullptr) {
		Log::warn("Connect event received for null peer");
		return;
	}
	// Check if the peer is already connected
	if (event.peer->state == ENET_PEER_STATE_CONNECTED) {
		Log::warn("Connect event received for already connected peer");
		return;
	}

	Log::trace("Connect event received for peer: " + std::to_string(event.peer->connectID));
	
	// From here, server has to wait for a CLIENT_CONNECT_BEGIN packet
}
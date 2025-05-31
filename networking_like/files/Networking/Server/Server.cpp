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

void Server::prepare_destroy() {
	LOG_SCOPE_SERVER;
	Log::trace("Preparing server for destruction");
	if (active) {
		stop().wait();
	}
	destroy_groups();
	enet_host_destroy(host);
	host = nullptr;
}

void Server::add_group(std::shared_ptr<SGroup> group) {
	LOG_SCOPE_SERVER;

	groups.push_back(group);
}

void Server::initialize_groups() {
	LOG_SCOPE_SERVER;
	Log::trace("Initializing groups!");
	
	// Built in protocols
	std::shared_ptr<SDisconnectGroup> disconnect = std::make_shared<SDisconnectGroup>(shared_from_this());
	disconnect_group = disconnect;
	add_group(disconnect);

	std::shared_ptr<SConnectGroup> connect = std::make_shared<SConnectGroup>(shared_from_this());
	add_group(connect);

	std::shared_ptr<SWorldGroup> world = std::make_shared<SWorldGroup>(shared_from_this());
	world_group = world;
	add_group(world);

	add_group(std::make_shared<SStateGroup>(shared_from_this()));
}

void Server::destroy_groups() {
	LOG_SCOPE_SERVER;
	Log::trace("Destroying groups");
	for (auto& g : groups) {
		g->deactivate();
	}
	groups.clear();
	disconnect_group = nullptr;
	world_group = nullptr;
}

void Server::broadcast_packet(const Packet& packet, std::vector<ENetPeer*> excluding) {
	LOG_SCOPE_SERVER;
	Log::trace("Broadcasting packet " + PacketHelper::types_to_string(packet) + " to all peers");
	for (auto& peer : peers.get_peers()) {
		if (peer.peer != nullptr && peer.peer->state != ENET_PEER_STATE_DISCONNECTED) {
			if (std::find(excluding.begin(), excluding.end(), peer.peer) == excluding.end()) {
				send_packet(packet, peer.peer);
			}
		}
	}
	enet_host_flush(host);
}

Server::~Server() {
	LOG_SCOPE_SERVER;
	Log::trace("Server destructor called");

	prepare_destroy();
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
	
	initialize_groups();

	if (active) {
		Log::warn("Server is already active, cannot start");
		return;
	}

	active = true;
	update_future = std::async(std::launch::async, &Server::update_loop, this);
	Log::trace("Server started update loop.");

	Events::Server::Start::trigger(Events::Server::StartData()); // Fire event
}

std::future<ShutdownResult> Server::stop() {
	LOG_SCOPE_SERVER;

	if (!active) {
		Log::warn("Server is not active, cannot stop");
		return std::async(std::launch::async, []() { 
			return ShutdownResult(ShutdownResultType::FAILURE, 0, "Server is already inactive.", 0, 0);
		});
	}

	return std::async(std::launch::async, &Server::stop_thread, this);
}

ShutdownResult Server::stop_thread() {
	LOG_SCOPE_SERVER;
	Log::trace("Server stop thread started");
	
	// Send disconnect packets to all peers, and add pending disconnects
	uint64_t start_time = TimeUtils::get_current_time_millis();
	uint8_t peers_graceful = 0;
	uint8_t peers_forced = 0;
	std::vector<std::future<DisconnectResult>> disconnect_futures;

	// Send disconnect packets to all peers
	for (auto& peer : peers.get_peers()) {
		if (peer.peer != nullptr && peer.peer->state != ENET_PEER_STATE_DISCONNECTED) {
			// Add future to track disconnect result
			disconnect_futures.push_back(disconnect_peer(peer.peer, DisconnectResultReason::SERVER_STOPPING));
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

	// Stop update() loop
	active = false;
	if (update_future.valid() && std::this_thread::get_id() != update_thread_id) {
		update_future.get(); // Wait for the update loop to finish
	}
	else {
		Log::warn("Update loop is in the same thread, cannot stop()");
	}

	// Fire event
	Events::Server::Stop::trigger(Events::Server::StopData()); // Fire event

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

std::future<DisconnectResult> Server::disconnect_peer(ENetPeer* peer, DisconnectResultReason reason) {
	LOG_SCOPE_SERVER;
	Log::trace("Disconnecting peer: " + peers.get_polite_handle(peer));

	// Verify we have the disconnect protocol
	if (!disconnect_group) {
		Log::error("Disconnect group not initialized!");
		return std::async(std::launch::async, [reason]() {
			return DisconnectResult(DisconnectResultType::FAILED, reason, "Disconnect group not initialized", 0);
			});
	}

	// Use the protocol to disconnect the peer
	return std::async(std::launch::async, [this, peer, reason]() {
		DisconnectResult result = disconnect_group->disconnect_peer(peer, reason);

		return result;
		});
}

void Server::update() {
	LOG_SCOPE_SERVER;
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		Events::Server::EventReceive::trigger(Events::Server::EventReceiveData(event)); // Fire event
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

	Events::Server::Update::trigger(Events::Server::UpdateData());
}
// Events

void Server::disconnect_event(const ENetEvent& event) {
	LOG_SCOPE_SERVER;
}

void Server::receive_event(const ENetEvent& event) {
	LOG_SCOPE_SERVER;
}

void Server::connect_event(const ENetEvent& event) {
	LOG_SCOPE_SERVER;
}

bool Server::send_packet(const Packet& packet, ENetPeer* peer) {
	LOG_SCOPE_SERVER;

	if (packet.header.direction != PacketDirection::SERVER_TO_CLIENT) {
		Log::error("Invalid packet header direction, must be S->C");
	}

	Log::trace("Sending packet " + PacketHelper::types_to_string(packet) + " to peer " + peers.get_polite_handle(peer));

	return NetworkUser::send_packet(packet, peer);
}

std::shared_ptr<SWorldGroup> Server::get_world_group() {
	return world_group;
}
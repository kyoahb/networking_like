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
		ConnectResult cr = connect_protocol->connect(ip, port, preferred_handle);
		start();
		return cr;
	});
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
	start_protocols();
}

void Client::init() {
	initialize_protocols();
}

void Client::stop() {
	LOG_SCOPE_CLIENT;

	if (!active) {
		Log::warn("Client update loop is not running, cannot stop");
		return;
	}

	NetworkUser::stop();
	stop_protocols();
}

void Client::update() {
	LOG_SCOPE_CLIENT;
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		dispatch_event_to_protocols(event); // Dispatch event to protocols
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
	update_protocols();
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

// Protocols

void Client::add_protocol(std::shared_ptr<CProtocol> protocol) {
	LOG_SCOPE_CLIENT;
	protocols.push_back(protocol);
}


void Client::initialize_protocols() {
	LOG_SCOPE_CLIENT;
	// Add built-in protocols here

	std::shared_ptr<CConnect> connect_p = std::make_shared<CConnect>(shared_from_this());
	connect_protocol = connect_p;
	add_protocol(connect_p);
}

void Client::start_protocols() {
	LOG_SCOPE_CLIENT;
	for (auto& protocol : protocols) {
		protocol->start();
	}
}

void Client::stop_protocols() {
	LOG_SCOPE_CLIENT;
	for (auto& protocol : protocols) {
		protocol->stop();
	}
}

void Client::update_protocols() {
	LOG_SCOPE_CLIENT;
	for (auto& protocol : protocols) {
		protocol->update();
	}
}

void Client::destroy_protocols() {
	LOG_SCOPE_CLIENT;
	for (auto& protocol : protocols) {
		protocol->destroy();
	}
	protocols.clear();
}

void Client::dispatch_event_to_protocols(const ENetEvent& event) {
	LOG_SCOPE_CLIENT;
	for (auto& protocol : protocols) {
		protocol->packet_event(event);
	}
}



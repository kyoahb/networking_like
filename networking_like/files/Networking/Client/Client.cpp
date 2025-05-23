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

void Client::prepare_destroy() {
	LOG_SCOPE_CLIENT;
	if (is_connected()) {
		Log::warn("Client is still connected, disconnecting...");
		disconnect().wait();
	}
	if (active) {
		stop();
	}
	destroy_groups();
	enet_host_destroy(host);
	host = nullptr;
}

bool Client::is_connected() {
	LOG_SCOPE_CLIENT;
	return peers.is_connected();
}

// CONNECTION LOGIC

std::future<ConnectResult> Client::connect(const std::string& ip, uint16_t port, const std::string& preferred_handle) {
	LOG_SCOPE_CLIENT;
	Log::trace("Connecting to server at " + ip + ":" + std::to_string(port) + " with preferred handle: " + preferred_handle);

	if (groups.empty()) {
		initialize_groups();
	}

	return std::async(std::launch::async, [this, ip, port, preferred_handle]() {
		ConnectResult cr = connect_group->connect(ip, port, preferred_handle);
		if (cr.type == ConnectResultType::SUCCESS) start();
		return cr;
	});
}


// DISCONNECT LOGIC

std::future<DisconnectResult> Client::disconnect() {
	LOG_SCOPE_CLIENT;
	Log::trace("Disconnecting from server...");

	return std::async(std::launch::async, [this]() {
		return disconnect_group->disconnect(DisconnectResultReason::USER_REQUESTED);
	});
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
	

	return NetworkUser::send_packet(packet, peers.get_server().peer);
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

	Events::Client::Start::trigger(Events::Client::StartData());
}

void Client::stop() {
	LOG_SCOPE_CLIENT;

	if (!active) {
		Log::warn("Client update loop is not running, cannot stop");
		return;
	}

	NetworkUser::stop();

	Events::Client::Stop::trigger(Events::Client::StopData());
}

void Client::update() {
	LOG_SCOPE_CLIENT;
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		Events::Client::EventReceive::trigger(Events::Client::EventReceiveData(event));
	}
	Events::Client::Update::trigger(Events::Client::UpdateData());
}

// Protocols

void Client::add_group(std::shared_ptr<CGroup> group) {
	LOG_SCOPE_CLIENT;
	groups.push_back(group);
}


void Client::initialize_groups() {
	LOG_SCOPE_CLIENT;
	// Add built-in groups here

	connect_group = std::make_shared<CConnectGroup>(shared_from_this());
	add_group(connect_group);
	disconnect_group = std::make_shared<CDisconnectGroup>(shared_from_this());
	add_group(disconnect_group);
}


void Client::destroy_groups() {
	LOG_SCOPE_CLIENT;
	for (auto& g : groups) {
		g->deactivate();
	}
	groups.clear();
}

#include "CConnectGroup.h"
#include "Networking/Client/Client.h"
CConnectGroup::CConnectGroup(std::shared_ptr<Client> _client) : CGroup(_client) {
	activate();
}

CConnectGroup::~CConnectGroup() {
	deactivate();
	// Destructor implementation
}

void CConnectGroup::activate() {
	// Add event callbacks
	event_receive_callback = Events::Client::EventReceive::register_callback([this](const Events::Client::EventReceiveData& data) { on_event_receive(data); });
}

void CConnectGroup::deactivate() {
	// Remove event callbacks
	
	Events::Client::EventReceive::unregister_callback(event_receive_callback);
	client = nullptr;


}

void CConnectGroup::on_event_receive(const Events::Client::EventReceiveData& data) {
	// Event callback for receiving events
	// Handle the event here
	LOG_SCOPE_CLIENT_GROUP;

	ENetEvent event = data.event;

	if (event.type != ENET_EVENT_TYPE_RECEIVE) return;
	Packet packet(event.packet);
	if (packet.header.type != PacketType::CLIENT_CONNECT) return;


	if (packet.header.subtype == CLIENT_CONNECT_RELAY) {
		// Received a CLIENT_CONNECT_RELAY packet
		ClientConnectRelay client_connect_relay = SerializationUtils::deserialize<ClientConnectRelay>(packet.data, packet.header.size);
		client->peers.add_peer(client_connect_relay.client_id, client_connect_relay.client_handle, client_connect_relay.is_host);
		Log::trace("CLIENT_CONNECT:RELAY received. Added peer: " + client_connect_relay.client_handle);

		std::optional<LocalNetPeer> peer = client->peers.get_peer(client_connect_relay.client_id);
		if (!peer.has_value()) {
			Log::error("CLIENT_CONNECT:RELAY received, but peer not found in peerlist.");
		}
		else {
			Events::Client::PeerAdded::trigger(Events::Client::PeerAddedData(peer.value()));
		}
	}


}

ConnectResult CConnectGroup::connect(const std::string& ip, uint16_t port, const std::string& preferred_handle) {
	LOG_SCOPE_CLIENT_GROUP;

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
		Log::error("Connection to server failed: could not establish connection");
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
	Events::Client::Connect::trigger(Events::Client::ConnectData(server_peer, ip, static_cast<int>(port)));
	return ConnectResult{ ConnectResultType::SUCCESS, "Connected to server", TimeUtils::get_current_time_millis() - start_time };
}

ENetPeer* CConnectGroup::begin_connection(const std::string& ip, uint16_t port, const std::string& preferred_handle) {
	LOG_SCOPE_CLIENT_GROUP;

	enet_address_set_host(&client->address, ip.c_str());
	client->address.port = port;

	ENetPeer* server_peer = enet_host_connect(client->host, &client->address, 2, 0);

	Log::trace("Attempting connection to server at " + ip + ":" + std::to_string(port) + " with preferred handle: " + preferred_handle);
	return server_peer; // Check if valid will be done in connect()
}

bool CConnectGroup::wait_for_connection_establishment(ENetPeer* server_peer) {
	LOG_SCOPE_CLIENT;
	if (server_peer == nullptr) {
		Log::error("Server peer is nullptr, cannot wait for connection establishment");
		return false;
	}

	const unsigned int ESTABLISHMENT_TIMEOUT = 100;
	Log::trace("Waiting for connection establishment for up to " + ESTABLISHMENT_TIMEOUT);
	std::optional<ENetEvent> event = NetworkHelper::wait_for_event(client->host, ESTABLISHMENT_TIMEOUT, ENET_EVENT_TYPE_CONNECT);

	if (event.has_value()) {
		Log::trace("Connection establishment successful. Connected to server");
		return true;
	}

	Log::trace("Connection establishment FAILED.");
	return false;
}

void CConnectGroup::send_connection_begin_packet(const std::string& preferred_handle) {
	LOG_SCOPE_CLIENT;

	ClientConnectBegin connect_begin;
	connect_begin.client_preferred_handle = preferred_handle;

	std::string serialised_data = SerializationUtils::serialize<ClientConnectBegin>(connect_begin);
	Packet packet(PacketType::CLIENT_CONNECT, PacketDirection::CLIENT_TO_SERVER, ClientConnectType::CLIENT_CONNECT_BEGIN, serialised_data.data(), serialised_data.size(), true);

	client->send_packet(packet);
}

bool CConnectGroup::wait_for_connection_confirmation() {
	LOG_SCOPE_CLIENT;

	const unsigned int TIMEOUT_MS = 5000;

	Log::trace("Waiting up to " + std::to_string(TIMEOUT_MS) + "ms for connection confirmation...");
	std::optional<ENetEvent> event = NetworkHelper::wait_for_event(client->host, TIMEOUT_MS, PacketType::CLIENT_CONNECT, PacketDirection::SERVER_TO_CLIENT, static_cast<uint8_t>(ClientConnectType::CLIENT_CONNECT_CONFIRM));

	if (event.has_value()) {
		Packet received_packet(event.value().packet);
		ClientConnectConfirm connection_confirmation = SerializationUtils::deserialize<ClientConnectConfirm>(received_packet.data, received_packet.header.size);
		client->peers.setup_by_confirmation(connection_confirmation);

		Log::trace("CLIENT_CONNECT:CONFIRM received. Server-given handle for US: " + connection_confirmation.client_decided_handle);

		return true;
	}
	Log::error("Connection confirmation timed out");
	return false;
}
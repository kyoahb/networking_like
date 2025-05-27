#include "CDisconnectGroup.h"
#include "Networking/Client/Client.h"
#include "Networking/Packets/Data/ClientDisconnect.h"

CDisconnectGroup::CDisconnectGroup(std::shared_ptr<Client> _client) : CGroup(_client) {
	LOG_SCOPE_CLIENT_GROUP;
	activate();
};

CDisconnectGroup::~CDisconnectGroup() {
	LOG_SCOPE_CLIENT_GROUP;
	deactivate();
};

void CDisconnectGroup::activate() {
	LOG_SCOPE_CLIENT_GROUP;
	update_callback = Events::Client::Update::register_callback([this](const Events::Client::UpdateData& data) { update(data); });
	event_receive_callback = Events::Client::EventReceive::register_callback([this](const Events::Client::EventReceiveData& data) { event_receive(data); });
};

void CDisconnectGroup::deactivate() {
	LOG_SCOPE_CLIENT_GROUP;
	Events::Client::Update::unregister_callback(update_callback);
	Events::Client::EventReceive::unregister_callback(event_receive_callback);
	client = nullptr;
};

void CDisconnectGroup::update(const Events::Client::UpdateData& data) {
	LOG_SCOPE_CLIENT_GROUP;
	// Check for pending disconnect
	if (pending_disconnect.has_value()) {
		if (TimeUtils::get_time_since(pending_disconnect->request_time) >= DISCONNECT_TIMEOUT) {
			Log::warn("Client has timed out waiting for disconnect confirmation. Timeout was " + std::to_string(DISCONNECT_TIMEOUT) + "ms");
			pending_disconnect.reset();

			force_disconnect();

		}
	}
}

void CDisconnectGroup::event_receive(const Events::Client::EventReceiveData& data) {
	LOG_SCOPE_CLIENT_GROUP;
	const ENetEvent& event = data.event;
	
	if (event.type != ENET_EVENT_TYPE_RECEIVE) return; // Only care about packets
	if (event.packet == nullptr) return; // No packet, no event
	Packet p(event.packet); // Create packet from event
	if (p.header.type != PacketType::CLIENT_DISCONNECT) return; // Only care about CLIENT_CONNECT packets

	if (p.header.subtype == ClientDisconnectType::CLIENT_DISCONNECT_BEGIN) {
		// The server has requested that we disconnect.
		Log::trace("CLIENT_DISCONNECT_BEGIN packet received from server, disconnecting");
		pending_disconnect.reset(); // Reset disconnect

		// Gathering the reason
		ClientDisconnectBegin client_disconnect_begin = SerializationUtils::deserialize<ClientDisconnectBegin>(p.data, p.header.size);
		DisconnectResultReason reason = client_disconnect_begin.reason;
		DisconnectResultType type = client_disconnect_begin.type;
		std::string reason_string = client_disconnect_begin.message;

		DisconnectResult result;
		result.type = type;
		result.reason = reason;
		result.message = reason_string;
		result.time_taken = 0; // We have no idea

		// Trigger event before we disconnect everything
		Events::Client::Disconnect::trigger(Events::Client::DisconnectData(result));

		// Return a confirm packet to let server know we have received the message
		ClientDisconnectConfirm client_disconnect_confirm;
		std::string serialized = SerializationUtils::serialize<ClientDisconnectConfirm>(client_disconnect_confirm);
		Packet confirm_packet(PacketType::CLIENT_DISCONNECT, PacketDirection::CLIENT_TO_SERVER, ClientDisconnectType::CLIENT_DISCONNECT_CONFIRM, serialized.data(), serialized.size(), true);
		client->send_packet(confirm_packet); // Send confirm packet to server
		enet_host_flush(client->host); // Flush the host to ensure the packet is sent immediately
		Log::trace("CLIENT_DISCONNECT_CONFIRM sent to server. Now removing server_peer");

		// Just in case
		enet_peer_disconnect(client->peers.get_server().peer, 0); // Send disconnect packet to server just to let it know
		enet_peer_reset(client->peers.get_server().peer); // Reset the server peer

		// Disconnect the client
		client->peers.clear();
		client->stop();


	}
	else if (p.header.subtype == ClientDisconnectType::CLIENT_DISCONNECT_RELAY) {
		// Received a CLIENT_DISCONNECT_RELAY packet
		ClientDisconnectRelay client_disconnect_relay = SerializationUtils::deserialize<ClientDisconnectRelay>(p.data, p.header.size);

		std::optional<LocalNetPeer> peer = client->peers.get_peer(client_disconnect_relay.client_id);
		if (!peer.has_value()) {
			Log::error("CLIENT_DISCONNECT:RELAY received, but peer not found in peerlist.");
		}
		else {
			Log::trace("CLIENT_DISCONNECT:RELAY received. Removed peer: " + client->peers.get_polite_handle(client_disconnect_relay.client_id));
			client->peers.remove_peer(client_disconnect_relay.client_id); // Remove peer from the peerlist
			Events::Client::PeerRemoved::trigger(Events::Client::PeerRemovedData(peer.value()));
		}
	}
	// NO CONFIRM checking: this is handled in disconnect()
}

DisconnectResult CDisconnectGroup::force_disconnect() {
	LOG_SCOPE_CLIENT_GROUP;
	Log::trace("Forcing disconnect");

	// Trigger event
	DisconnectResult result;
	result.type = DisconnectResultType::FORCED;
	result.reason = DisconnectResultReason::USER_REQUESTED;
	result.message = "Client requested disconnect";
	result.time_taken = 0; // We dont care
	Events::Client::Disconnect::trigger(Events::Client::DisconnectData(result));

	enet_peer_disconnect(client->peers.get_server().peer, 0); // Send disconnect packet to server just to let it know
	enet_peer_reset(client->peers.get_server().peer); // Reset the server peer
	client->peers.clear();
	client->stop();

	return result;
}

void CDisconnectGroup::add_pending_disconnect(DisconnectResultReason reason) {
	LOG_SCOPE_CLIENT_GROUP;
	Log::trace("Adding pending disconnect");
	pending_disconnect = PendingDisconnect(reason);
	pending_disconnect->request_time = TimeUtils::get_current_time_millis();
}

DisconnectResult CDisconnectGroup::disconnect(DisconnectResultReason reason) {
	LOG_SCOPE_CLIENT_GROUP;
	// ENSURE THIS IS EXECUTED IN ASYNC! THIS IS BLOCKING

	Log::trace("Disconnecting client");

	DisconnectResult result;
	result.reason = reason;
	// Not connected, pointless
	if (!client->is_connected()) {
		Log::warn("Client is not connected, cannot disconnect");
		result.type = DisconnectResultType::FAILED;
		return result;
	}


	// Add pending disconnect
	add_pending_disconnect(reason);
	client->stop(); // Stop the client, this will block until the server disconnects us

	// Send ClientDisconnectBegin packet
	ClientDisconnectBegin client_disconnect_begin;
	client_disconnect_begin.type = DisconnectResultType::UNKNOWN;
	client_disconnect_begin.reason = reason;
	client_disconnect_begin.message = "Client requested disconnect";
	std::string serialized = SerializationUtils::serialize<ClientDisconnectBegin>(client_disconnect_begin);
	Packet disconnect_packet(PacketType::CLIENT_DISCONNECT, PacketDirection::CLIENT_TO_SERVER, ClientDisconnectType::CLIENT_DISCONNECT_BEGIN, serialized.data(), serialized.size(), true);
	client->send_packet(disconnect_packet); // Send disconnect packet to server

	// BLocking
	// Wait for disconnect confirmation
	uint64_t start_time = TimeUtils::get_current_time_millis();
	std::optional<ENetEvent> event = NetworkHelper::wait_for_event(client->host, DISCONNECT_TIMEOUT, PacketType::CLIENT_DISCONNECT, PacketDirection::ANY_DIRECTION, ClientDisconnectType::CLIENT_DISCONNECT_CONFIRM);

	if (event.has_value()) {
		Log::trace("Disconnect confirmation received");
		result.type = DisconnectResultType::SUCCESS;

		Events::Client::Disconnect::trigger(Events::Client::DisconnectData(result));

		// Send regular disconnect event just in case
		enet_peer_disconnect(client->peers.get_server().peer, 0); // Send disconnect packet to server just to let it know
		enet_peer_reset(client->peers.get_server().peer); // Reset the server peer
		client->peers.clear();
		return result;
	}

	// If we are still here, we did not receive a confirmation
	Log::error("Disconnect confirmation timed out");
	result.type = DisconnectResultType::FORCED;

	Events::Client::Disconnect::trigger(Events::Client::DisconnectData(result));

	enet_peer_disconnect(client->peers.get_server().peer, 0); // Send disconnect packet to server just to let it know
	enet_peer_reset(client->peers.get_server().peer); // Reset the server peer
	client->peers.clear();
	return result;






}
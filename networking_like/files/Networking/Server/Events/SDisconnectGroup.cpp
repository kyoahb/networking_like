#include "SDisconnectGroup.h"
#include "Networking/Server/Server.h"
#include "Networking/Packets/Data/ClientDisconnect.h"

SDisconnectGroup::SDisconnectGroup(std::shared_ptr<Server> _server) : SGroup(_server) {
	LOG_SCOPE_SERVER_GROUP;
	activate();
};

SDisconnectGroup::~SDisconnectGroup() {
	LOG_SCOPE_SERVER_GROUP;
	deactivate();
};

void SDisconnectGroup::activate() {
	LOG_SCOPE_SERVER_GROUP;
	update_callback = Events::Server::Update::register_callback([this](const Events::Server::UpdateData& data) { update(data); });
	event_receive_callback = Events::Server::EventReceive::register_callback([this](const Events::Server::EventReceiveData& data) { event_receive(data); });
};

void SDisconnectGroup::deactivate() {
	LOG_SCOPE_SERVER_GROUP;
	Events::Server::Update::unregister_callback(update_callback);
	Events::Server::EventReceive::unregister_callback(event_receive_callback);

	server = nullptr;
};

void SDisconnectGroup::update(const Events::Server::UpdateData& data) {
	LOG_SCOPE_SERVER_GROUP;
	// Check for pending disconnects
	for (auto it = pending_disconnects.begin(); it != pending_disconnects.end();) {
		if (TimeUtils::get_time_since(it->second.request_time) >= NetworkConstants::ConnectionTimeout) {
			Log::warn("Disconnect packet timed out for peer: " + server->peers.get_polite_handle(it->first));
			it = pending_disconnects.erase(it);
		}
		else {
			it++;
		}
	}
}

void SDisconnectGroup::event_receive(const Events::Server::EventReceiveData& data) {
	LOG_SCOPE_SERVER_GROUP;

	const ENetEvent& event = data.event;

	std::string handle = server->peers.get_polite_handle(event.peer);

	if (event.type != ENET_EVENT_TYPE_RECEIVE) return;
	Packet packet(event.packet);
	if (packet.header.type != PacketType::CLIENT_DISCONNECT) return;

	if (packet.header.subtype == ClientDisconnectType::CLIENT_DISCONNECT_BEGIN) {
		// The client wants to disconnect from us
		ClientDisconnectBegin client_disconnect_begin = SerializationUtils::deserialize<ClientDisconnectBegin>(packet.data, packet.header.size);
		DisconnectResultReason reason = client_disconnect_begin.reason;
		DisconnectResultType type = client_disconnect_begin.type;
		std::string reason_string = client_disconnect_begin.message;
		
		// Gathering the reason
		Log::trace("CLIENT_DISCONNECT:BEGIN received from " + handle + ", reason: " + reason_string);

		// Send relay
		send_relay_to_others(event.peer);

		// Return Confirm
		ClientDisconnectConfirm client_disconnect_confirm;
		std::string serialized = SerializationUtils::serialize<ClientDisconnectConfirm>(client_disconnect_confirm);
		Packet confirm_packet(PacketType::CLIENT_DISCONNECT, PacketDirection::SERVER_TO_CLIENT, ClientDisconnectType::CLIENT_DISCONNECT_CONFIRM, serialized.data(), serialized.size());
		server->send_packet(confirm_packet, event.peer); // Send confirm packet to client
		Log::trace("CLIENT_DISCONNECT:CONFIRM sent to " + handle);

		// Event
		DisconnectResult result;
		result.type = type;
		result.reason = reason;
		result.message = reason_string;
		result.time_taken = 0; // We have no idea

		// Trigger event before we disconnect everything
		Events::Server::ClientDisconnect::trigger(Events::Server::ClientDisconnectData(event.peer, result));

		// Disconnect the peer
		server->peers.remove_peer(event.peer); // Remove the peer from the peerlist
		enet_peer_reset(event.peer);
	}
	else if (packet.header.subtype == ClientDisconnectType::CLIENT_DISCONNECT_CONFIRM) {
		// Received a CLIENT_DISCONNECT_CONFIRM packet
		if (pending_disconnects.find(event.peer) == pending_disconnects.end()) {
			Log::warn("CLIENT_DISCONNECT:CONFIRM received from unregistered peer: " + handle);
			return;
		}
		Log::trace("CLIENT_DISCONNECT:CONFIRM received from peer: " + handle + " erasing pending disconnect.");
		pending_disconnects.erase(event.peer);
	}

	

}

void SDisconnectGroup::send_relay_to_others(ENetPeer* excluded) {
	LOG_SCOPE_SERVER_GROUP;
	std::optional<NetPeer> peer = server->peers.get_peer(excluded);
	if (peer.has_value()) {
		// Send relay
		ClientDisconnectRelay client_disconnect_relay;
		client_disconnect_relay.client_id = peer.value().id;
		std::string serialized = SerializationUtils::serialize<ClientDisconnectRelay>(client_disconnect_relay);
		Packet relay_packet(PacketType::CLIENT_DISCONNECT, PacketDirection::SERVER_TO_CLIENT, ClientDisconnectType::CLIENT_DISCONNECT_RELAY, serialized.data(), serialized.size());
		server->broadcast_packet(relay_packet, { excluded }); // Send relay packet to all clients except the one that sent it
	}
	else {
		// DO NOT send relay.
		// This is because if the server does not have the peer saved in ServerPeerlist
		// The client has disconnected in the middle of the connection process
		// (Just after Sending CONNECTION request, before receiving CLIENT_CONNECT_BEGIN)
		// Therefore, other clients have no record of this client
	}
}

void SDisconnectGroup::add_pending_disconnect(ENetPeer* peer, DisconnectResultReason reason) {
	LOG_SCOPE_SERVER_GROUP;

	PendingDisconnect p;
	p.reason = reason;
	p.type = DisconnectResultType::UNKNOWN;
	p.request_time = TimeUtils::get_current_time_millis();

	pending_disconnects[peer] = p;
}

DisconnectResult SDisconnectGroup::disconnect_peer(ENetPeer* peer, DisconnectResultReason reason) {
	LOG_SCOPE_SERVER_GROUP;

	// Checks
	if (peer == nullptr) {
		Log::error("Peer is null, cannot disconnect");
		return DisconnectResult(DisconnectResultType::FAILED, reason, "Peer is null", 0);
	}
	if (!server->peers.is_peer_connected(peer)) {
		Log::error("Peer is not connected, cannot disconnect");
		return DisconnectResult(DisconnectResultType::FAILED, reason, "Peer is not connected", 0);
	}
	std::string handle = server->peers.get_polite_handle(peer);
	if (pending_disconnects.find(peer) != pending_disconnects.end()) {
		Log::error("Peer " + handle + " is already pending disconnect, cannot disconnect");
		return DisconnectResult(DisconnectResultType::FAILED, reason, "Peer is already pending disconnect", 0);
	}
	

	// Add pending disconnect
	Log::trace("Added pending disconnect for peer: " + handle);
	add_pending_disconnect(peer, reason);

	// Send a CLIENT_DISCONNECT_BEGIN packet to the client
	ClientDisconnectBegin client_disconnect_begin;
	client_disconnect_begin.type = DisconnectResultType::UNKNOWN;
	client_disconnect_begin.reason = reason;
	client_disconnect_begin.message = "Server requested disconnect";
	std::string serialized = SerializationUtils::serialize<ClientDisconnectBegin>(client_disconnect_begin);

	Packet disconnect_packet(PacketType::CLIENT_DISCONNECT, PacketDirection::SERVER_TO_CLIENT, ClientDisconnectType::CLIENT_DISCONNECT_BEGIN, serialized.data(), serialized.size());
	server->send_packet(disconnect_packet, peer);
	Log::trace("CLIENT_DISCONNECT:BEGIN sent to peer: " + handle + ", reason: " + client_disconnect_begin.message);

	// Wait for CLIENT_DISCONNECT_CONFIRM packet
	// (BLOCKING, ensure this is ran in async)
	bool accepted = false;
	uint64_t start_time = TimeUtils::get_current_time_millis();
	ENetEvent event;
	Log::trace("Waiting for CLIENT_DISCONNECT:CONFIRM from peer: " + handle);
	while (TimeUtils::get_time_since(start_time) < NetworkConstants::ConnectionTimeout) {
		if (pending_disconnects.find(peer) == pending_disconnects.end()) {
			accepted = true;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(NetworkConstants::UpdateSleepMs));
	}

	DisconnectResult result;
	result.time_taken = TimeUtils::get_time_since(start_time);
	result.reason = reason;

	if (accepted) {
		result.type = DisconnectResultType::SUCCESS;
		result.message = "Client disconnected successfully";

		Log::trace("CLIENT_DISCONNECT:CONFIRM was received from peer: " + handle);
	}
	else {
		result.type = DisconnectResultType::FORCED;
		result.message = "Client disconnect confirmation timed out";

		Log::warn("CLIENT_DISCONNECT:CONFIRM timed out for peer: " + handle + ". They will be 'force' disconnected.");
	}

	send_relay_to_others(peer);

	server->peers.remove_peer(peer); // Remove the peer from the peerlist
	Events::Server::ClientDisconnect::trigger(Events::Server::ClientDisconnectData(peer, result));
	enet_peer_reset(peer);

	return result;
}
#include "SDisconnectGroup.h"
#include "Networking/Server/Server.h"

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
};

void SDisconnectGroup::update(const Events::Server::UpdateData& data) {
	LOG_SCOPE_SERVER_GROUP;
	// Check for pending disconnects
	for (auto it = pending_disconnects.begin(); it != pending_disconnects.end();) {
		if (TimeUtils::get_time_since(it->second.request_time) >= DISCONNECT_TIMEOUT) {
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

	if (event.type != ENET_EVENT_TYPE_DISCONNECT) return;

	// Check if the peer is valid
	if (event.peer == nullptr) {
		Log::warn("Cannot disconnect peer, peer is nullptr");
		return;
	}

	Log::trace("Disconnect event received for peer: " + handle + ". Broadcasting to all peers");

	// Send disconnect relay to all other peers
	ClientDisconnectRelay relay;
	relay.client_id = event.peer->incomingPeerID; // same as peer.value().id but skips optional check
	std::string serialized = SerializationUtils::serialize<ClientDisconnectRelay>(relay);
	Packet relay_packet(PacketType::CLIENT_CONNECT, PacketDirection::SERVER_TO_CLIENT, ClientConnectType::CLIENT_DISCONNECT_RELAY, serialized.data(), serialized.size(), true);
	server->broadcast_packet(relay_packet, { event.peer }); // Send to all peers except the one that disconnected


	DisconnectResult result;

	// Remove pending disconnect if exists
	auto it = pending_disconnects.find(event.peer);
	if (it != pending_disconnects.end()) {
		Log::trace("Removing Peer with pending disconnect that has sent a disconnect event: " + handle);

		// A pending disconnect found = we (server) requested it
		result.type = DisconnectResultType::SUCCESS;
		result.reason = it->second.reason;
		result.message = "Pending disconnect acknowledged";
		result.time_taken = TimeUtils::get_time_since(it->second.request_time);

		pending_disconnects.erase(it);
	}
	else {
		// No pending disconnect found means the peer requested it themselves
		result.type = DisconnectResultType::SUCCESS;
		result.reason = DisconnectResultReason::USER_REQUESTED;
		result.message = "Client disconnected";
		result.time_taken = 0; // We have no idea

		// We can fire an event here, because we know we wont be firing it elsewhere
		Events::Server::ClientDisconnect::trigger(Events::Server::ClientDisconnectData(event.peer, result)); // Fire disconnect event
	}
	server->peers.remove_peer(event.peer); // Remove peer from the server's peer list

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
	LOG_SCOPE_SERVER_PROTOCOL;

	// Check if the peer is valid
	if (peer == nullptr) {
		Log::warn("Cannot disconnect peer, peer is nullptr");
		return DisconnectResult(DisconnectResultType::FAILED, reason, "Peer is nullptr", 0);
	}

	std::string peer_handle = server->peers.get_polite_handle(peer);

	// Check if the peer is already disconnected
	if (peer->state == ENET_PEER_STATE_DISCONNECTED) {
		Log::warn("Cannot disconnect peer, peer " + peer_handle + " is already disconnected");
		return DisconnectResult(DisconnectResultType::FAILED, reason, "Peer is already disconnected", 0);
	}

	Log::trace("Beginning disconnect_peer() for peer: " + peer_handle);


	// Send disconnect packet to the peer
	enet_peer_disconnect(peer, static_cast<uint8_t>(reason));
	Log::trace("Sent disconnect packet to peer: " + peer_handle + ". Now waiting for a response (pending disconnect)");
	// Add pending disconnect
	add_pending_disconnect(peer, reason);

	int start_time = TimeUtils::get_current_time_millis();
	bool acknowledged = false;

	while (!acknowledged && TimeUtils::get_time_since(start_time) < DISCONNECT_TIMEOUT) {
		// Check if still in pending disconnects.
		// If it isnt, then update() has received a disconnect event and handled it
		if (pending_disconnects.find(peer) == pending_disconnects.end()) {
			acknowledged = true;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(CHECK_INTERVAL));
		Log::trace("Waiting for disconnect confirmation from peer: " + peer_handle + " " + std::to_string(TimeUtils::get_time_since(start_time)) + "ms");
	}

	DisconnectResult result;
	result.reason = reason;
	result.time_taken = TimeUtils::get_time_since(start_time);
	if (acknowledged) {
		// Successful
		pending_disconnects.erase(peer);
		result.type = DisconnectResultType::SUCCESS;
		result.message = "Peer disconnected successfully";
		Log::trace("Peer disconnected successfully: " + peer_handle);
	}
	else {
		// Failed, forceful disconnect required
		enet_peer_reset(peer);
		pending_disconnects.erase(peer);
		result.type = DisconnectResultType::FORCED;
		result.message = "Peer disconnected forcefully : Time limit exceeded";
		Log::trace("Peer disconnected forcefully, time limit exceeded: " + peer_handle);
	}
	server->peers.remove_peer(peer);
	Events::Server::ClientDisconnect::trigger(Events::Server::ClientDisconnectData(peer, result)); // Fire disconnect event
	return result;
}
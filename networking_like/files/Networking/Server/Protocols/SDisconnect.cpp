#include "SDisconnect.h"
#include "Networking/Server/Server.h"
#include "Networking/Packets/Data/ClientDisconnectRelay.h"

SDisconnect::SDisconnect(std::shared_ptr<Server> _server) : SProtocol(_server, "SDisconnect") {
	LOG_SCOPE_SERVER_PROTOCOL;
}

SDisconnect::~SDisconnect() {
	LOG_SCOPE_SERVER_PROTOCOL;
}

void SDisconnect::packet_event(const ENetEvent& event, std::optional<NetPeer> peer) {
	LOG_SCOPE_SERVER_PROTOCOL;

	if (event.type != ENET_EVENT_TYPE_DISCONNECT) return;

	// Check if the peer is valid
	if (event.peer == nullptr) {
		Log::warn("Cannot disconnect peer, peer is nullptr");
		return;
	}

	Log::trace("Disconnect event received for peer: " + std::to_string(event.peer->incomingPeerID) + ". Broadcasting to all peers");

	// Send disconnect relay to all other peers
	ClientDisconnectRelay relay;
	relay.client_id = event.peer->incomingPeerID; // same as peer.value().id but skips optional check
	std::string serialized = SerializationUtils::serialize<ClientDisconnectRelay>(relay);
	Packet relay_packet(PacketType::CLIENT_CONNECT, PacketDirection::SERVER_TO_CLIENT, ClientConnectType::CLIENT_DISCONNECT_RELAY, serialized.data(), serialized.size(), true);
	server->broadcast_packet(relay_packet, { event.peer }); // Send to all peers except the one that disconnected

	// Remove pending disconnect if exists
	// TODO: Check if equality check is possible using NetPeer like this
	if (peer.has_value()) {
		auto it = pending_disconnects.find(event.peer);
		if (it != pending_disconnects.end()) {
			Log::trace("Removing Peer with pending disconnect that has sent a disconnect event: " + peer.value().handle);
			pending_disconnects.erase(it);
		}
		server->peers.remove_peer(event.peer); // Remove peer from the server's peer list
	}
	else { 
		// Peer does not have a value yet. This means either the server received a disconnect event
		// Before the client sent the CLIENT_CONNECT_BEGIN packet, or something else has gone wrong
		if (!server->peers.get_peer(event.peer).has_value()) {
			// Not much to do, server has no record of it so it is pointless removing it, and peer likely does not have 
		}
	}

	// Construct result
	DisconnectResult result;
	result.type = DisconnectResultType::FORCED;
	result.reason = DisconnectResultReason::USER_REQUESTED;
	result.message = "Client disconnected";
	result.time_taken = 0; // We have no idea

	Events::Server::ClientDisconnect::trigger(Events::Server::ClientDisconnectData(event.peer, result)); // Fire disconnect event

}

void SDisconnect::add_pending_disconnect(ENetPeer* peer, DisconnectResultReason reason) {
	LOG_SCOPE_SERVER_PROTOCOL;
	pending_disconnects[peer] = { reason, DisconnectResultType::UNKNOWN, TimeUtils::get_current_time_millis() };
}

void SDisconnect::update() {
	// Work pending disconnects
	LOG_SCOPE_SERVER_PROTOCOL;

	if (TimeUtils::get_time_since(last_check_time) >= CHECK_INTERVAL) {
		work_pending_disconnects();
		last_check_time = TimeUtils::get_current_time_millis();
	}
}

void SDisconnect::work_pending_disconnects() {
	LOG_SCOPE_SERVER_PROTOCOL;
	for (auto it = pending_disconnects.begin(); it != pending_disconnects.end();) {
		if (TimeUtils::get_time_since(it->second.request_time) > DISCONNECT_TIMEOUT) {
			// We wait double the time out limit, as work_pending_disconnects() is not expected
			// To manage forceful disconnects. Rather, the disconnect_peer_thread() should do this.
			// However, if there is an issue, then after double the timeout limit (plenty of time)
			// We remove the pending disconnect here.
			ENetPeer* peer = it->first;
			Log::warn("Pending disconnect timed out & unmanaged for peer: " + server->peers.get_polite_handle(peer));
			server->peers.remove_peer(peer);

			// Construct result
			DisconnectResult result;
			result.type = DisconnectResultType::FORCED;
			result.reason = it->second.reason;
			result.message = "Pending disconnect timed out, but unmanaged.";
			result.time_taken = TimeUtils::get_time_since(it->second.request_time);

			Events::Server::ClientDisconnect::trigger(Events::Server::ClientDisconnectData(peer, result)); // Fire disconnect event

			it = pending_disconnects.erase(it);
		}
		else {
			it++;
		}
	}
}

DisconnectResult SDisconnect::disconnect_peer(ENetPeer* peer, DisconnectResultReason reason) {
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

void SDisconnect::start() {
	LOG_SCOPE_SERVER_PROTOCOL;
}

void SDisconnect::stop() {
	LOG_SCOPE_SERVER_PROTOCOL;
	pending_disconnects.clear();
}

void SDisconnect::destroy() {
	LOG_SCOPE_SERVER_PROTOCOL;
	pending_disconnects.clear();
}
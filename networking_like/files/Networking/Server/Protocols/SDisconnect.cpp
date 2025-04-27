#include "SDisconnect.h"

SDisconnect::SDisconnect(std::shared_ptr<Server> _server) : SProtocol(_server, "SDisconnect") {
	LOG_SCOPE_SERVER_PROTOCOL;
}

SDisconnect::~SDisconnect() {
	LOG_SCOPE_SERVER_PROTOCOL;
}

void SDisconnect::packet_event(const ENetEvent& event, std::optional<NetPeer> peer) {
	LOG_SCOPE_SERVER_PROTOCOL;

	// Check if the peer is valid
	if (event.peer == nullptr) {
		Log::warn("Cannot disconnect peer, peer is nullptr");
		return;
	}

	// Get NetPeer
	if (!peer.has_value()) {
		Log::warn("Disconnect event received for unknown peer");
		return;
	}

	if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
		Log::trace("Disconnect event received for peer: " + peer.value().handle);

		// Remove pending disconnect if exists
		auto it = pending_disconnects.find(peer.value());
		if (it != pending_disconnects.end()) {
			Log::trace("Pending disconnect found for peer: " + peer.value().handle);
			pending_disconnects.erase(it);
		}
	}
}

void SDisconnect::add_pending_disconnect(NetPeer peer, DisconnectResultReason reason) {
	LOG_SCOPE_SERVER_PROTOCOL;
	Log::trace("Adding pending disconnect for peer: " + peer.handle);
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
			Log::warn("Pending disconnect timed out & unmanaged for peer: " + it->first.handle);
			enet_peer_reset(it->first.peer);
			it = pending_disconnects.erase(it);
		}
		else {
			it++;
		}
	}
}

DisconnectResult SDisconnect::disconnect_peer(const NetPeer& peer, DisconnectResultReason reason) {
	LOG_SCOPE_SERVER_PROTOCOL;
	Log::trace("Disconnecting peer thread: " + peer.handle);

	// Check if the peer is valid
	if (peer.peer == nullptr) {
		Log::warn("Cannot disconnect peer, peer is nullptr");
		return DisconnectResult(DisconnectResultType::FAILED, reason, "Peer is nullptr", 0);
	}
	if (peer.peer->state == ENET_PEER_STATE_DISCONNECTED) {
		Log::warn("Cannot disconnect peer, peer is already disconnected");
		return DisconnectResult(DisconnectResultType::FAILED, reason, "Peer is already disconnected", 0);
	}

	// Add pending disconnect
	add_pending_disconnect(peer, reason);
	// Send disconnect packet to the peer
	enet_peer_disconnect(peer.peer, static_cast<uint8_t>(reason));

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
	}

	DisconnectResult result;
	result.reason = reason;
	result.time_taken = TimeUtils::get_time_since(start_time);
	if (acknowledged) {
		// Successful
		result.type = DisconnectResultType::SUCCESS;
		result.message = "Peer disconnected successfully";
	}
	else {
		// Failed, forceful disconnect required
		enet_peer_reset(peer.peer);
		pending_disconnects.erase(peer);
		result.type = DisconnectResultType::FORCED;
		result.message = "Peer disconnected forcefully : Time limit exceeded";
	}

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
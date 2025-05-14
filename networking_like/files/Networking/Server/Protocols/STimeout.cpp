#include "STimeout.h"
#include "Networking/Server/Server.h"
STimeout::STimeout(std::shared_ptr<Server> _server) : SProtocol(_server, "STimeout") {
	// Constructor implementation
}

STimeout::~STimeout() {
	// Destructor implementation
}

void STimeout::packet_event(const ENetEvent& event) {
	
	if (event.peer == nullptr) return;

	peer_last_event[event.peer] = TimeUtils::get_current_time_millis(); // Update the last event time for the peer
}

void STimeout::update() {
	LOG_SCOPE_SERVER_PROTOCOL;

	// Check for timeouts
	for (auto it = peer_last_event.begin(); it != peer_last_event.end();) {
		if (TimeUtils::get_time_since(it->second) >= TIMEOUT_MS) {
			Log::warn("Peer timed out: " + server->peers.get_polite_handle(it->first));

			// Disconnect peer
			server->disconnect_peer(it->first, DisconnectResultReason::TIMEOUT);
			it = peer_last_event.erase(it);
		}
		else {
			it++;
		}
	}
}

bool STimeout::request_heartbeat(ENetPeer* peer) {
	LOG_SCOPE_SERVER_PROTOCOL;

	if (peer == nullptr) return false;

	// Send a heartbeat packet to the peer
	Packet packet;
	packet.header.type = PacketType::HEARTBEAT;
	packet.header.subtype = HeartbeatType::HEARTBEAT_REQUEST;
	packet.header.direction = PacketDirection::SERVER_TO_CLIENT;

	server->send_packet(packet, peer);

	// Wait for heartbeat response (handled in packet_event)


	return true;
}
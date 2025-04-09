#include "NetworkUser.h"

NetworkUser::NetworkUser() {};

void NetworkUser::start() {
	LOG_SCOPE_NET;
	if (active) {
		Log::warn("NetworkUser is already active, cannot start again");
		return;
	}

	active = true;
	update_future = std::async(std::launch::async, &NetworkUser::update_loop, this);
}

void NetworkUser::update_loop() {
	update_thread_id = std::this_thread::get_id();
	while (active) {
		update();
		std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL));
	}
}

bool NetworkUser::send_packet(const Packet& packet, ENetPeer* peer) {
	LOG_SCOPE_NET;

	// Check peer validity
	if (peer == nullptr) {
		Log::error("Cannot send packet, peer is nullptr");
		return false;
	}

	std::string handle = static_cast<char*>(peer->data);
	NetPeer net_peer(peer, 0, handle);

	return send_packet(packet, net_peer);
}

bool NetworkUser::send_packet(const Packet& packet, const NetPeer& peer) {
	LOG_SCOPE_NET;
	Log::trace("Sending packet (type: " + std::to_string(packet.header.type) + " subtype: " + std::to_string(packet.header.subtype) + " to peer : " + peer.handle);

	// Check packet validity
	if (!packet.is_valid) {
		Log::error("Cannot send packet, invalid packet");
		return false;
	}

	// Check formed enetpacket validity
	ENetPacket* enet_packet = packet.to_enet_packet();
	if (enet_packet == nullptr) {
		Log::error("Failed to create ENetPacket from Packet");
		return false;
	}

	// Check result of sending packet
	int result = enet_peer_send(peer.peer, 0, enet_packet);
	if (result < 0) {
		Log::error("Failed to send packet to peer: " + peer.handle);
		return false;
	}

	return true;
}

void NetworkUser::add_handler_callback(PacketType type, std::function<void(const ENetEvent&, const Packet&)> callback) {
	handlers[type] = callback;
}


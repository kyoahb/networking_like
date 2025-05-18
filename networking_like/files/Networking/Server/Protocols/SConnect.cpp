#include "SConnect.h"
#include "Networking/Server/Server.h"

SConnect::SConnect(std::shared_ptr<Server> _server) : SProtocol(_server, "SConnect") {
	LOG_SCOPE_SERVER_PROTOCOL;
};

SConnect::~SConnect() {
	LOG_SCOPE_SERVER_PROTOCOL;
}

void SConnect::on_update() {
	LOG_SCOPE_SERVER_PROTOCOL;
	// Check for pending begins
	for (auto it = pending_begins.begin(); it != pending_begins.end();) {
		if (TimeUtils::get_time_since(it->second) >= TIMEOUT_MS) {
			Log::warn("CLIENT_CONNECT_BEGIN packet timed out for peer: " + std::to_string(it->first->incomingPeerID));
			
			// Disconnect peer
			server->disconnect_peer(it->first, DisconnectResultReason::TIMEOUT);
			it = pending_begins.erase(it);
		}
		else {
			it++;
		}
	}
}

void SConnect::packet_event(const ENetEvent& event) {
	LOG_SCOPE_SERVER_PROTOCOL;

	std::string peer_handle = server->peers.get_polite_handle(event.peer);

	if (event.type == ENET_EVENT_TYPE_CONNECT) {
		Log::trace("Connect event received for peer: " + peer_handle + " , waiting for CLIENT_CONNECT_BEGIN packet");
		if (server->peers.get_peer(event.peer).has_value()) {
			Log::warn("Connect event received for known peer: " + peer_handle);
			return;
		}

		// A new client has connected, that the server has no record of.
		// We now add peer to the set of peers we expect a CLIENT_CONNECT_BEGIN packet from
		pending_begins[event.peer] = TimeUtils::get_current_time_millis();

		// Fire relevant event
		Events::Server::ClientConnect::trigger(Events::Server::ClientConnectData(event.peer));

	}
	else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
		Packet packet(event.packet);

		if (!(
			packet.header.type == PacketType::CLIENT_CONNECT && 
			packet.header.subtype == ClientConnectType::CLIENT_CONNECT_BEGIN)) return;

		// Received a CLIENT_CONNECT_BEGIN packet

		if (!pending_begins.contains(event.peer)) {
			Log::warn("Received CLIENT_CONNECT_BEGIN packet from unregistered begins peer: " + peer_handle);
		}
		else {
			Log::trace("Received CLIENT_CONNECT_BEGIN packet from peer: " + peer_handle);
			pending_begins.erase(event.peer);
		}

		ClientConnectBegin client_connect_begin = SerializationUtils::deserialize<ClientConnectBegin>(packet.data, packet.header.size);

		// Get final handle
		std::string client_decided_handle = client_connect_begin.client_preferred_handle;

		// There is already a peer with this handle, we force the clients name to be unique
		while (server->peers.get_peer(client_decided_handle).has_value()) {
			client_decided_handle = client_connect_begin.client_preferred_handle + "_" + std::to_string(rand() % 1000);
		}

		// Can now construct the NetPeer, and attach it to the server
		NetPeer new_peer(event.peer, event.peer->incomingPeerID, client_decided_handle);
		server->peers.add_peer(new_peer);

		// Now return a CLIENT_CONNECT_CONFIRM packet
		ClientConnectConfirm client_connect_confirm;
		client_connect_confirm.server_preferred_handle = server->peers.self.handle;
		client_connect_confirm.client_decided_handle = client_decided_handle;
		client_connect_confirm.client_id = new_peer.id;
		client_connect_confirm.other_clients = netpeer_list_to_relay_list(server->peers.get_peers(), event.peer);

		std::string serialised_data = SerializationUtils::serialize<ClientConnectConfirm>(client_connect_confirm);
		Packet packet_confirm(PacketType::CLIENT_CONNECT, PacketDirection::SERVER_TO_CLIENT, ClientConnectType::CLIENT_CONNECT_CONFIRM, serialised_data.data(), serialised_data.size(), true);
		server->send_packet(packet_confirm, event.peer);

		// Send a CLIENT_CONNECT_RELAY packet to all other clients
		ClientConnectRelay client_connect_relay = netpeer_to_relay(new_peer);
		std::string serialised_relay_data = SerializationUtils::serialize<ClientConnectRelay>(client_connect_relay);
		Packet packet_relay(PacketType::CLIENT_CONNECT, PacketDirection::SERVER_TO_CLIENT, ClientConnectType::CLIENT_CONNECT_RELAY, serialised_relay_data.data(), serialised_relay_data.size(), true);
		server->broadcast_packet(packet_relay, { event.peer });
		
		
		// Fire event
		Events::Server::ClientBegin::trigger(Events::Server::ClientBeginData(event.peer, client_connect_begin));
	}
}

ClientConnectRelay SConnect::netpeer_to_relay(const NetPeer& peer) {
	LOG_SCOPE_SERVER_PROTOCOL;
	ClientConnectRelay relay;
	relay.client_id = peer.id;
	relay.client_handle = peer.handle;
	return relay;
}

std::vector<ClientConnectRelay> SConnect::netpeer_list_to_relay_list(const std::vector<NetPeer>& peers, ENetPeer* exclude) {
	LOG_SCOPE_SERVER_PROTOCOL;
	std::vector<ClientConnectRelay> relays;
	for (const auto& peer : peers) {
		if (peer.peer != exclude)
		relays.push_back(netpeer_to_relay(peer));
	}
	return relays;
}




// unused overrides

void SConnect::on_start() {
	LOG_SCOPE_SERVER_PROTOCOL;
}

void SConnect::on_stop() {
	LOG_SCOPE_SERVER_PROTOCOL;
}

void SConnect::on_destroy() {
	LOG_SCOPE_SERVER_PROTOCOL;
}
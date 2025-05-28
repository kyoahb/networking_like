#include "SConnectGroup.h"
#include "Networking/Server/Server.h"
#include "Utils/General/RandomUtils.h"
SConnectGroup::SConnectGroup(std::shared_ptr<Server> _server) : SGroup(_server) {
    activate();
};

SConnectGroup::~SConnectGroup() {
    deactivate();
};

void SConnectGroup::activate() {
	update_callback = Events::Server::Update::register_callback([this](const Events::Server::UpdateData& data) { update(data); });
	event_receive_callback = Events::Server::EventReceive::register_callback([this](const Events::Server::EventReceiveData& data) { event_receive(data); });
};

void SConnectGroup::deactivate() {
	Events::Server::Update::unregister_callback(update_callback);
	Events::Server::EventReceive::unregister_callback(event_receive_callback);

    server = nullptr;
};

void SConnectGroup::update(const Events::Server::UpdateData& data) {
	LOG_SCOPE_SERVER_GROUP;
	

	for (auto it = pending_begins.begin(); it != pending_begins.end();) {
		if (TimeUtils::get_time_since(it->second) >= CONNECTBEGIN_TIMEOUT) {
			Log::warn("CLIENT_CONNECT_BEGIN packet timed out for peer: " + server->peers.get_polite_handle(it->first));

			// Disconnect peer
			server->disconnect_peer(it->first, DisconnectResultReason::TIMEOUT);
			it = pending_begins.erase(it);
		}
		else {
			it++;
		}
	}
}

void SConnectGroup::event_receive(const Events::Server::EventReceiveData& data) {
    LOG_SCOPE_SERVER_GROUP;
    const ENetEvent& event = data.event;
    std::string peer_handle = server->peers.get_polite_handle(event.peer);

    if (event.type == ENET_EVENT_TYPE_CONNECT) {
        Log::trace("Connect event received for peer: " + peer_handle + " , waiting for CLIENT_CONNECT_BEGIN packet");
        if (server->peers.is_peer_connected(event.peer)) {
            Log::warn("Connect event received for connected peer: " + peer_handle);
            return;
        }

		// A new client has connected, that the server has no record of.
        pending_begins[event.peer] = TimeUtils::get_current_time_millis();
        Events::Server::ClientConnect::trigger(Events::Server::ClientConnectData(event.peer));
    }



    else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
        Packet packet(event.packet);
        if (!(packet.header.type == PacketType::CLIENT_CONNECT && packet.header.subtype == ClientConnectType::CLIENT_CONNECT_BEGIN))
            return;

        if (!pending_begins.contains(event.peer)) {
            Log::warn("Received CLIENT_CONNECT_BEGIN packet from unregistered begins peer: " + peer_handle);
        }
        else {
            Log::trace("Received CLIENT_CONNECT_BEGIN packet from peer: " + peer_handle);
            pending_begins.erase(event.peer);
        }

        ClientConnectBegin client_connect_begin = SerializationUtils::deserialize<ClientConnectBegin>(packet.data, packet.header.size);

		uint8_t client_id = get_next_id();
		std::string client_decided_handle = get_handle(client_connect_begin.client_preferred_handle, client_id);

        NetPeer new_peer(event.peer, client_id, client_decided_handle);
        server->peers.add_peer(new_peer);

		std::optional<NetPeer> p = server->peers.get_peer(event.peer);
		Log::asserts(p.has_value(), "New peer should be added to server peer list, but was not found after adding");



        // Send CLIENT_CONNECT_CONFIRM
        ClientConnectConfirm client_connect_confirm;
        client_connect_confirm.server_preferred_handle = server->peers.get_self().handle;
        client_connect_confirm.is_host = p.value().is_host; // Is the connected client the server host?
        client_connect_confirm.client_decided_handle = client_decided_handle;
        client_connect_confirm.client_id = client_id;
        client_connect_confirm.other_clients = netpeer_list_to_relay_list(server->peers.get_peers(), event.peer);

        std::string serialised_data = SerializationUtils::serialize<ClientConnectConfirm>(client_connect_confirm);
        Packet packet_confirm(PacketType::CLIENT_CONNECT, PacketDirection::SERVER_TO_CLIENT, ClientConnectType::CLIENT_CONNECT_CONFIRM, serialised_data.data(), serialised_data.size(), true);
        server->send_packet(packet_confirm, event.peer);

        // Send CLIENT_CONNECT_RELAY to others
        ClientConnectRelay client_connect_relay = netpeer_to_relay(p.value());
        std::string serialised_relay_data = SerializationUtils::serialize<ClientConnectRelay>(client_connect_relay);
        Packet packet_relay(PacketType::CLIENT_CONNECT, PacketDirection::SERVER_TO_CLIENT, ClientConnectType::CLIENT_CONNECT_RELAY, serialised_relay_data.data(), serialised_relay_data.size(), true);
        server->broadcast_packet(packet_relay, { event.peer });

        Events::Server::ClientBegin::trigger(Events::Server::ClientBeginData(event.peer, client_connect_begin));
    }
}

// Helper methods
ClientConnectRelay SConnectGroup::netpeer_to_relay(const NetPeer& peer) {
    ClientConnectRelay relay;
    relay.client_id = peer.id;
    relay.client_handle = peer.handle;
	relay.is_host = peer.is_host;
    return relay;
}

std::vector<ClientConnectRelay> SConnectGroup::netpeer_list_to_relay_list(const std::vector<NetPeer>& peers, ENetPeer* exclude) {
    std::vector<ClientConnectRelay> relays;
    for (const auto& peer : peers) {
        if (peer.peer != exclude)
            relays.push_back(netpeer_to_relay(peer));
    }
    return relays;
}

uint8_t SConnectGroup::get_next_id() {
    uint8_t id = next_id;
    if (next_id >= 254) {
        throw std::runtime_error("ID overflow: server has had too many peers, please reset the server");
    }

	next_id++;

	return id;
}

std::string SConnectGroup::get_handle(const std::string& preferred_handle, uint8_t id) {
	std::string handle = preferred_handle;
    
	// Check if handle is already taken
	while (server->peers.is_peer_connected(handle)) {
        // If taken, append a random string
		handle = preferred_handle + "_" + RandomUtils::random_string(4);
		// Possible (theoretically) that all of these are taken but it's unlikely
	}

	return handle;
}
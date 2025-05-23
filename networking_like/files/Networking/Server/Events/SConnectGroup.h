#pragma once

#include "Utils/Imports/common.h"
#include "SGroup.h"
#include "Utils/General/TimeUtils.h"
#include "Networking/Packets/Data/ClientConnect.h"

class SConnectGroup : public SGroup {
public:
	SConnectGroup(std::shared_ptr<Server> _server);
	~SConnectGroup();

	// Overrides
	void activate() override; // Group activated. In here, add event callbacks
	void deactivate() override; // Group deactivated. In here, remove event callbacks

	void update(const Events::Server::UpdateData& data); // Connected to Events::Server::Update
	void event_receive(const Events::Server::EventReceiveData& data); // Connected to Events::Server::EventReceive

private:
	int update_callback;
	int event_receive_callback;

	uint8_t next_id = 0; // Next ID to assign to a new client
	uint8_t get_next_id(); // Get the next ID to assign to a new client

	std::string get_handle(const std::string& preferred_handle, uint8_t id); // Get a handle for the client, based on the preferred handle

	std::unordered_map<ENetPeer*, uint64_t> pending_begins; // Set of peers that are pending a CLIENT_CONNECT_BEGIN packet
	const unsigned int CONNECTBEGIN_TIMEOUT = 5000; // Timeout for client connection

	// Helper methods
	ClientConnectRelay netpeer_to_relay(const NetPeer& peer);
	std::vector<ClientConnectRelay> netpeer_list_to_relay_list(const std::vector<NetPeer>& peers, ENetPeer* exclude);
};

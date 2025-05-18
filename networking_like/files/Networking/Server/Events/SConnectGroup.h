#pragma once

#include "Utils/Imports/common.h"
#include "SGroup.h"
#include "Utils/General/TimeUtils.h"
#include "Networking/Packets/Data/ClientConnect.h"
#include "Networking/Packets/Data/ClientConnectBegin.h"
#include "Networking/Packets/Data/ClientConnectConfirm.h"
#include "Networking/Packets/Data/ClientConnectRelay.h"

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


	std::unordered_map<ENetPeer*, uint64_t> pending_begins; // Set of peers that are pending a CLIENT_CONNECT_BEGIN packet
	const unsigned int CONNECTBEGIN_TIMEOUT = 5000; // Timeout for client connection

	// Helper methods
	ClientConnectRelay netpeer_to_relay(const NetPeer& peer);
	std::vector<ClientConnectRelay> netpeer_list_to_relay_list(const std::vector<NetPeer>& peers, ENetPeer* exclude);
};

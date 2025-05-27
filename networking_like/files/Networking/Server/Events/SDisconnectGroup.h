#pragma once

#include "Utils/Imports/common.h"
#include "SGroup.h"
#include "Utils/General/TimeUtils.h"
#include "Game/Events/EventList.h"
#include "Networking/Packets/Data/ClientConnect.h"
#include "Networking/Shared/FutureResults/DisconnectResult.h"

class SDisconnectGroup : public SGroup {
public:
	SDisconnectGroup(std::shared_ptr<Server> _server);
	~SDisconnectGroup();

	DisconnectResult disconnect_peer(ENetPeer* peer, DisconnectResultReason reason = DisconnectResultReason::SERVER_REQUESTED); // Disconnect a peer

	void activate() override; // Group activated. In here, add event callbacks
	void deactivate() override; // Group deactivated. In here, remove event callbacks

	void update(const Events::Server::UpdateData& data); // Connected to Events::Server::Update
	void event_receive(const Events::Server::EventReceiveData& data); // Connected to Events::Server::EventReceive
private:
	int update_callback;
	int event_receive_callback;
	std::unordered_map<ENetPeer*, PendingDisconnect> pending_disconnects; // Set of peers that are pending a CLIENT_CONNECT_BEGIN packet
	const int DISCONNECT_TIMEOUT = 500; // Timeout for client connection
	const int CHECK_INTERVAL = 100; // Check interval in milliseconds

	void add_pending_disconnect(ENetPeer* peer, DisconnectResultReason reason); // Add a pending disconnect
	void send_relay_to_others(ENetPeer* excluded);
};
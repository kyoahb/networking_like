#pragma once

#include "CGroup.h"
#include "Networking/Packets/Data/ClientDisconnect.h"

class CDisconnectGroup : public CGroup {
public:
	CDisconnectGroup(std::shared_ptr<Client> _client);
	~CDisconnectGroup();
	// Overrides
	void activate() override; // Group activated. In here, add event callbacks
	void deactivate() override; // Group deactivated. In here, remove event callbacks
	void update(const Events::Client::UpdateData& data); // Connected to Events::Client::Update
	void event_receive(const Events::Client::EventReceiveData& data); // Connected to Events::Client::EventReceive

	DisconnectResult force_disconnect(); // Reset server peer without sending request.
	DisconnectResult disconnect(DisconnectResultReason reason = DisconnectResultReason::USER_REQUESTED); // Disconnect the client, waiting for server to acknowledge it.

private:
	int update_callback;
	int event_receive_callback;
	std::optional<PendingDisconnect> pending_disconnect; // Exists if client is waiting to be disconnected.
	const unsigned int DISCONNECT_TIMEOUT = 500; // Timeout for disconnect
	const unsigned int CHECK_INTERVAL = 100; // Check interval for disconnect

	void add_pending_disconnect(DisconnectResultReason reason); // Add a pending disconnect
};

#pragma once

#include "Utils/Imports/common.h"
#include "SProtocol.h"
#include "Networking/Packets/Data/Heartbeat.h"
#include "Networking/Packets/Data/HeartbeatRequest.h"
#include "Networking/Packets/Data/HeartbeatResponse.h"
class STimeout : public SProtocol {
public:
	STimeout(std::shared_ptr<Server> _server);
	~STimeout();

	void packet_event(const ENetEvent& event) override; // Called on receive event
	void update() override;
private:
	bool request_heartbeat(ENetPeer* peer);
	const unsigned int TIMEOUT_MS = 5000;
	std::unordered_map<ENetPeer*, uint64_t> peer_last_event = {};
};

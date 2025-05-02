#pragma once

#include "Utils/Imports/common.h"
#include "Networking/Shared/NetPeer.h"
#include "Networking/Packets/Packet.h"

#include "Networking/Shared/Protocol.h"

class Server;

class SProtocol : public Protocol {
public:
	explicit SProtocol(std::shared_ptr<Server> _server, std::string _name);
	virtual ~SProtocol() = default;

	virtual void packet_event(const ENetEvent& event, std::optional<NetPeer> peer) = 0; // Called on receive event

protected:
	std::shared_ptr<Server> server = nullptr; // Pointer to the server instance
};

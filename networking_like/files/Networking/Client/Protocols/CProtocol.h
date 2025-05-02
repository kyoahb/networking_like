#pragma once
#include "Utils/Imports/common.h"
#include "Networking/Shared/Protocol.h"


class Client;

class CProtocol : public Protocol {
public:
	explicit CProtocol(std::shared_ptr<Client> _client, std::string _name);
	virtual ~CProtocol() = default;

	virtual void packet_event(const ENetEvent& event) = 0; // Called on receive event. NO netpeer as it is always the server.

protected:
	std::shared_ptr<Client> client = nullptr; // Pointer to the client instance
};
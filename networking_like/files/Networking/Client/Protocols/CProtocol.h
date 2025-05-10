#pragma once
#include "Utils/Imports/common.h"
#include "Networking/Shared/Protocol.h"

class Client;

class CProtocol : public Protocol {
public:
	explicit CProtocol(std::shared_ptr<Client> _client, std::string _name);
	virtual ~CProtocol();

	virtual void packet_event(const ENetEvent& event) = 0; // Called on receive event. NO netpeer as it is always the server.

	virtual void on_start() override; // Called on start
	virtual void on_stop() override; // Called on stop
	virtual void on_update() override; // Called on update
	virtual void on_destroy() override; // Called on destroy

	void start() override; // Start the protocol
	void stop() override; // Stop the protocol
	void update() override; // Update the protocol
	void destroy() override; // Destroy the protocol

	void cleanup(); // Cleanup function to be called on destruction
protected:
	std::shared_ptr<Client> client = nullptr; // Pointer to the client instance
};
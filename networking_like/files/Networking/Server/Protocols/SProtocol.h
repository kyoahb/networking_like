#pragma once

#include "Utils/Imports/common.h"
#include "Networking/Shared/NetPeer.h"
#include "Networking/Packets/Packet.h"

#include "Networking/Shared/Protocol.h"
#include "Game/Events/EventList.h"
class Server;

class SProtocol : public Protocol {
public:
	explicit SProtocol(std::shared_ptr<Server> _server, std::string _name);
	virtual ~SProtocol();

	virtual void packet_event(const ENetEvent& event); // Called on receive event

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
	std::shared_ptr<Server> server = nullptr; // Pointer to the server instance
};

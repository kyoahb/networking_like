#pragma once

#include "Utils/Imports/common.h"
#include "Networking/Shared/NetPeer.h"

class SProtocol {
public:
	explicit SProtocol(std::string _name);
	virtual ~SProtocol() = default;

	virtual void init() = 0; // Initialize the protocol
	virtual void start() = 0; // Called on server start()
	virtual void stop() = 0; // Called on server stop()
	virtual void update() = 0; // Called on server update()
	virtual void destroy() = 0; // Called on server destructor
	virtual void receive_event(const ENetEvent& event, std::optional<NetPeer> peer) = 0; // Called on receive event

	const std::string& get_name() const;

protected:
	std::string name = "Unnamed SProtocol"; // Name of the protocol
};

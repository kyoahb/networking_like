#pragma once

#include "Utils/Imports/common.h"
#include "Game/Events/EventList.h"
class Client;

class CGroup {
public:
	CGroup(std::shared_ptr<Client> _client);
	~CGroup();

	virtual void activate(); // Group activated. In here, add event callbacks
	virtual void deactivate(); // Group deactivated. In here, remove event callbacks

protected:
	std::shared_ptr<Client> client;
};
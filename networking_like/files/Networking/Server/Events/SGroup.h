#pragma once


#include "Utils/Imports/common.h"
#include "Game/Events/EventList.h"
class Server;

class SGroup {
public:
	SGroup(std::shared_ptr<Server> _server);
	~SGroup();
	
	virtual void activate(); // Group activated. In here, add event callbacks
	virtual void deactivate(); // Group deactivated. In here, remove event callbacks

protected:
	std::shared_ptr<Server> server;
};
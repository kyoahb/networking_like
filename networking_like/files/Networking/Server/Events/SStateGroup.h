#pragma once

#include "SGroup.h"
#include "Game/World/WorldObject.h"
#include "Networking/Packets/Data/WorldInfo.h"
class SStateGroup : public SGroup
{
public:
	SStateGroup(std::shared_ptr<Server> _server);
	~SStateGroup();

	void activate() override;
	void deactivate() override;

	void event_receive(const Events::Server::EventReceiveData& data);
	void update(const Events::Server::UpdateData& data);

private:

	int event_receive_callback = -1;
	int update_callback = -1;
};

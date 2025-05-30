#pragma once

#include "CGroup.h"
#include "Networking/Shared/World/WorldObject.h"
#include "Networking/Packets/Data/WorldInfo.h"
class CWorldGroup : public CGroup
{
public:
	CWorldGroup(std::shared_ptr<Client> _client);
	~CWorldGroup();

	void activate() override;
	void deactivate() override;

	void event_receive(const Events::Client::EventReceiveData& data);
	void update(const Events::Client::UpdateData& data);

private:
	std::shared_ptr<Client> client;
	WorldObject world;

	int event_receive_callback = -1;
	int update_callback = -1;
};

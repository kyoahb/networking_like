#pragma once

#include "CGroup.h"
#include "Game/World/WorldObject.h"
#include "Networking/Packets/Data/WorldInfo.h"
class CStateGroup : public CGroup
{
public:
	CStateGroup(std::shared_ptr<Client> _client);
	~CStateGroup();

	void activate() override;
	void deactivate() override;

	void event_receive(const Events::Client::EventReceiveData& data);
	void update(const Events::Client::UpdateData& data);
	void change_state(const Events::Game::StateChangeData& data);
private:

	int event_receive_callback = -1;
	int update_callback = -1;
	int change_state_callback = -1;
};

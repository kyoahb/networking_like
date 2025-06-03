#pragma once

#include "CGroup.h"
#include "Game/World/WorldObject.h"
#include "Networking/Packets/Data/WorldInfo.h"
#include "Game/World/Entities/Player.h"

class CWorldGroup : public CGroup
{
public:
	CWorldGroup(std::shared_ptr<Client> _client);
	~CWorldGroup();

	void activate() override;
	void deactivate() override;

	void event_receive(const Events::Client::EventReceiveData& data);
	void update(const Events::Client::UpdateData& data);

	bool is_world_active(); // Returns whether the player is in the world
	void deactivate_world(); // Deactivate world, no longer accept packets about the world
	void begin_world(); // Instantiates player for self
	void update_world(); // Update world
	void update_player(); // Update self



private:
	WorldObject world;
	std::shared_ptr<Player> self;

	bool world_active = false;

	int event_receive_callback = -1;
	int update_callback = -1;
};

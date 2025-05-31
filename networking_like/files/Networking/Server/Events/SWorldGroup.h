#pragma once

#include "SGroup.h"
#include "Game/World/WorldObject.h"
#include "Networking/Packets/Data/WorldInfo.h"
class SWorldGroup : public SGroup
{
public:
	SWorldGroup(std::shared_ptr<Server> _server);
	~SWorldGroup();

	void generate_world(); // Create the world.
	void send_initial_world();
	void send_delta_world();

	void activate() override;
	void deactivate() override;

	void event_receive(const Events::Server::EventReceiveData& data);
	void update(const Events::Server::UpdateData& data);

private:
	WorldObject world;

	int event_receive_callback = -1;
	int update_callback = -1;

	void send_world();
};

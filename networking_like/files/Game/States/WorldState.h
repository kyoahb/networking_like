#pragma once

#include "Utils/Imports/common.h"
#include "Game/States/GameState.h"
#include "Game/Events/EventList.h"
#include "Game/World/WorldObject.h"

#include "Game/World/Entities/LocalPlayer.h"

class WorldState : public GameState {
public:
	WorldState(Game& _game);
	~WorldState();

	void event_receive(const Events::Client::EventReceiveData& data);
	void update(const Events::Client::UpdateData& data);

	void on_draw() override;
	void on_activate() override;
	void on_deactivate() override;

private:
	WorldObject world;

	std::shared_ptr<LocalPlayer> player;

	int event_receive_callback = -1;
	int update_callback = -1;

};
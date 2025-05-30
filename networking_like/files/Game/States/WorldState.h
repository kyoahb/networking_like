#pragma once

#include "Utils/Imports/common.h"
#include "Game/States/GameState.h"

class WorldState : public GameState {
public:
	WorldState(Game& _game);
	~WorldState();

	void on_draw() override;
	void on_activate() override;
	void on_deactivate() override;

};
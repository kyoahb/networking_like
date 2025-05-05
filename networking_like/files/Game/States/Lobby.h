#pragma once

#include "GameState.h"

class Lobby : public GameState {
public:
	Lobby(Game& _game);
	~Lobby();

	void on_draw() override;

private:
	rgc::Button backButton;
};

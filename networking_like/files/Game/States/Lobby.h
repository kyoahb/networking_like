#pragma once

#include "GameState.h"

class Lobby : public GameState {
public:
	Lobby();
	~Lobby();
	void draw() override;

private:
	rgc::Button backButton;
};

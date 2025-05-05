#pragma once
#include "Utils/Imports/common.h"

class Game;

class GameState {
public:
	GameState(Game& _game, const std::string& _name);
	~GameState();

	virtual void draw();
	void activate();
	void deactivate();
	bool isActive() const;

protected:
	Game& game;
	bool interactable = true;
	std::string name;
};
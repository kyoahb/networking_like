#pragma once
#include "Utils/Imports/common.h"

class Game;

class GameState {
public:
	GameState(Game& _game, const std::string& _name);
	~GameState();

	// Functions overriden by derived classes
	virtual void on_draw();
	virtual void on_activate();
	virtual void on_deactivate();


	// Reserved functions that call overriden ones, and do background work
	void draw();
	void activate();
	void deactivate();
	bool isActive() const;

protected:
	Game& game;
	bool interactable = true;
	std::string name;
};
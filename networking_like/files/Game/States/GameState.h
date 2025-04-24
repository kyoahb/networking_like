#pragma once
#include "Utils/Imports/common.h"

class GameState {
public:
	GameState(const std::string& _name);
	~GameState();

	virtual void update();
	virtual void draw();
	void activate();
	void deactivate();
	bool isActive() const;

protected:
	bool interactable = true;
	std::string name;
};
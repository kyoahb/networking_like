#pragma once

#include "Utils/Imports/common.h"

class Game {
public:
	Game();
	~Game();
	void begin();

private:
	void update();
	
	raylib::Window window;
};
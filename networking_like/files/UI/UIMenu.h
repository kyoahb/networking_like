#pragma once

#include "Utils/Imports/common.h"

class UIMenu {
public:
	UIMenu();

	virtual void draw();
	
	void activate();
	void deactivate();
protected:
	bool interactable = true;
};
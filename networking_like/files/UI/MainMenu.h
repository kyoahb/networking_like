#pragma once

#include "UIMenu.h"

class MainMenu : public UIMenu {
public:
	MainMenu();
	void show() override;
	void hide() override;
	void update() override;

private:


};

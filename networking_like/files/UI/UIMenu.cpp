#include "UIMenu.h"

UIMenu::UIMenu() {
	// Constructor implementation
}

void UIMenu::show() {
	visible = true;
}

void UIMenu::hide() {
	visible = false;
}

void UIMenu::update() {
	if (visible) {
		// Update logic for the menu
	}
}
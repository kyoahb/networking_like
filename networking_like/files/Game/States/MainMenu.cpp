#include "MainMenu.h"

MainMenu::MainMenu() : GameState("MainMenu") {
    titleLabel = rgc::Label(rgc::Bounds(raylib::Vector2(400, 100), raylib::Vector2(400, 80)), "Game");


    hostButton = rgc::Button(rgc::Bounds(raylib::Vector2(400, 250), raylib::Vector2(200, 50)), "Host");
    hostButton.OnClick([this]() {
        onHostButtonClick("127.0.0.1", 7777);
        });


    joinButton = rgc::Button(rgc::Bounds(raylib::Vector2(400, 320), raylib::Vector2(200, 50)), "Join");
    joinButton.OnClick([this]() {
        onJoinButtonClick("127.0.0.1", 7777);
        });


    exitButton = rgc::Button(rgc::Bounds(raylib::Vector2(400, 390), raylib::Vector2(200, 50)), "Exit");
    exitButton.OnClick([this]() {
        onExitButtonClick();
        });
}

MainMenu::~MainMenu() {
	// Destructor implementation
}

void MainMenu::draw() {
	// Draw the main menu
	titleLabel.Show();
	hostButton.Show();
	joinButton.Show();
	exitButton.Show();

	if (!interactable) return;

	titleLabel.Update();
	hostButton.Update();
	joinButton.Update();
	exitButton.Update();
}
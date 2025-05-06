#pragma once

#include "GameState.h"

class LobbyMember {
public:
	std::string name;
	const char* cName;
	int id;
	std::string idStr;
	const char* cId;
	int y_offset = 0; // Offset for drawing the member in the lobby

	rgc::Label nameLabel;
	rgc::Label idLabel;

	LobbyMember(const std::string& _name, int _id, int y_offset);
	
	void draw();
};

class Lobby : public GameState {
public:
	Lobby(Game& _game);
	~Lobby();

	void on_activate() override;
	void on_deactivate() override;
	void on_draw() override;

private:
	std::vector<LobbyMember> members;
	rgc::Button backButton;
	rgc::Panel lobbyPanel;
};

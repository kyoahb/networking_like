#pragma once

#include "GameState.h"

class LobbyMember {
private:
	std::string name;
	std::string idStr;

	int y_offset = 0;
	int start_x = 100;
	int start_y = 100;
	int size_x = 300;
	int size_y = 300;

	int id_offset_x = 100;
	int font_size = 20;

public:
	LobbyMember(std::string _name, int _id, int _y_offset);
	
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
};

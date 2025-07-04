#pragma once

#include "GameState.h"
#include "Game/Events/EventList.h"

class LobbyMember {
private:
	int y_offset = 0;
	int start_x = 100;
	int start_y = 100;
	int size_x = 300;
	int size_y = 300;

	int id_offset_x = 100;
	int font_size = 20;

public:
	LobbyMember(std::string _name, int _id, int _y_offset);
	
	std::string name;
	std::string idStr;

	void draw();
};

class Lobby : public GameState {
public:
	Lobby(Game& _game);
	~Lobby();

	void on_activate() override;
	void on_deactivate() override;
	void on_draw() override;

	void add_all_members();
	void on_peer_added(const Events::Client::PeerAddedData &data);
	void on_peer_removed(const Events::Client::PeerRemovedData& data);
	void on_client_disconnected(const Events::Client::DisconnectData& data);

private:
	std::vector<LobbyMember> members;

	int initial_y_offset = 50;
	int offset_change = 200; // Change per member

	int on_peer_added_callbackid = 0; // Need to save this in order to later unregister it
	int on_peer_removed_callbackid = 0; // Need to save this in order to later unregister it
	int on_client_disconnected_callbackid = 0; // Need to save this in order to later unregister it
};

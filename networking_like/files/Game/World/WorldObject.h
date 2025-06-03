#pragma once

#include "Networking/Packets/Data/WorldInfo.h"
#include "Entities/Player.h"
#include "Entities/Entity.h"
#include "TransmittableObjects.h"

class WorldObject {
public:
	std::unordered_map<unsigned int, std::shared_ptr<Player>> player_map;
	std::unordered_map<unsigned int, std::shared_ptr<Entity>> entity_map;

	WorldObject() = default;

};	void update_player(UpdateWorldPlayer update_packet); // Updates a player in the world 


class ClientWorldObject : public WorldObject {
	std::shared_ptr<Player> local_player; // The player that is controlled by the client

	ClientWorldObject() = default;

	// Client side
	void fill_world(SendFullWorld world_packet, const LocalNetPeer& self); // Fills the world with data from the server
	void update_world(UpdateWorldObject update_packet); // Updates an entity in the world based on data from the server
	void draw();

};

class ServerWorldObject : public WorldObject {
	ServerWorldObject(const std::vector<NetPeer>& peers);

	TransmittableWorld to_transmittable() const;
};
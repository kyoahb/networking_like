#include "WorldObject.h"

void WorldObject::update_player(UpdateWorldPlayer update_packet) {
	// todo
}



void ClientWorldObject::fill_world(SendFullWorld world_packet, const LocalNetPeer& self) {
	for (const auto& transmittable_P : world_packet.world.players) {
		auto player = std::make_shared<Player>(transmittable_P);
		if (transmittable_P.id == self.id) {
			local_player = player;
		}
		player_map[player->get_id()] = player;
	}

	for (const auto& transmittable_E : world_packet.world.entities) {
		auto entity = std::make_shared<Player>(transmittable_E);

		entity_map[entity->get_id()] = entity;
	}


	// Loop through all again, this time linking entity_ids to children

	for (const auto& transmittable_P : world_packet.world.players) {
		if (transmittable_P.children_ids.size() > 0) {
			std::shared_ptr<Player> player = 
			for (unsigned int child_id : transmittable_P.children_ids) {

			}
		}

	}

	for (const auto& transmittable_E : world_packet.world.entities) {
		auto entity = std::make_shared<Player>(transmittable_E);

		entity_map[entity->get_id()] = entity;
	}


}
#pragma once

#include "Networking/Packets/PacketData.h"
#include "Game/World/WorldObject.h"
enum WorldInfo : uint8_t {
	WORLD_ANY = 255,
	WORLD_SEND_FULL = 1
};

// Server -> Client
// Subtype 1 (WORLD_SEND_FULL)
class SendFullWorld: public PacketData {
public:
	TransmittableWorld world;

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(world);
	}
};

// Server -> Client
// Subtype 2 (WORLD_UPDATE_OBJ)
class UpdateWorldObject : public PacketData {
public:
	Entity entity;
	
	template <typename Archive>
	void serialize(Archive& archive) {
		archive(entity);
	}
};

// Server -> Client (about another player) AND Client -> Server (about local player)
// Subtype 3 (WORLD_UPDATE_PLAYER)
class UpdateWorldPlayer : public PacketData {
public:
	Player player;
	
	template <typename Archive>
	void serialize(Archive& archive) {
		archive(player);
	}
};

// Server -> Client
// Subtype 4 (WORLD_REQUEST_PLAYER)
class RequestWorldPlayer : public PacketData {
public:
	// No need for any ID, as it is the player that receives this packet we are requesting data from
	template <typename Archive>
	void serialize(Archive& archive) {
		archive(player_id);
	}
};

// Client -> Server
// Subtype 4 (WORLD_SET_PLAYER)
class UploadWorldPlayer : public PacketData {
public:
	Player player;
	template <typename Archive>
	void serialize(Archive& archive) {
		archive(player);
	}
};
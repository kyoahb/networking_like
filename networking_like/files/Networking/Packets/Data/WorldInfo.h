#pragma once

#include "Networking/Packets/PacketData.h"
#include "Networking/Shared/World/WorldObject.h"
enum WorldInfo : uint8_t {
	WORLD_ANY = 255,
	WORLD_FULL = 1
};

class WorldFull : public PacketData {
public:
	WorldObject world;

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(world);
	}
};
#pragma once
#include "Utils/Imports/common.h"

namespace cereal {
	template <class Archive>
	void serialize(Archive& archive, raylib::Vector3& v) {
		archive(v.x, v.y, v.z);
	}
}

struct TransmittableEntity {
	// DOES NOT TRANSMIT MODELS
	// MUST SAVEM ODELS LOCALLY AND CONNECT THEM TO AN ID OR SOMETHING


	unsigned int id;
	unsigned int parent_id;
	std::vector<unsigned int> children_ids;

	raylib::Vector3 position;
	raylib::Vector3 rotation;
	raylib::Vector3 scale;

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(id, parent_id, children_ids, position, rotation, scale);
	}
};

struct TransmittablePlayer : public TransmittableEntity {
	int peer_id;

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(static_cast<TransmittableEntity&>(*this), peer_id);
	}
};

// For WORLD_SEND_FULL
struct TransmittableWorld {
	std::vector<TransmittablePlayer> players;
	std::vector<TransmittableEntity> entities;

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(players, entities);
	}
};
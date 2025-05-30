#pragma once

struct WorldObject {
	int counter = 0;
	// TODO: Actually fill out with content

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(counter);
	}
};
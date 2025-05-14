#pragma once

#include "Utils/Imports/common.h"
#include "Networking/Packets/PacketData.h"

//1 S->C
class HeartbeatRequest : public PacketData {
public:
	template <typename Archive>
	void serialize(Archive& archive) {
		archive();
	}
};
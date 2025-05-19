#pragma once

#include "Utils/Imports/common.h"
#include "Networking/Packets/PacketData.h"

enum HeartbeatType : uint8_t {
	HEARTBEAT_ANY = 255,
	HEARTBEAT_REQUEST = 1,
	HEARTBEAT_RESPONSE = 2,
};

//1 S->C
class HeartbeatRequest : public PacketData {
public:
	template <typename Archive>
	void serialize(Archive& archive) {
		archive();
	}
};

//2 C->S
class HeartbeatResponse : public PacketData {
public:
	template <typename Archive>
	void serialize(Archive& archive) {
		archive();
	}
};
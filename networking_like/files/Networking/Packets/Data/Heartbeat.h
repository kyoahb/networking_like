#pragma once

#include "Utils/Imports/common.h"

enum HeartbeatType : uint8_t {
	HEARTBEAT_ANY = 255,
	HEARTBEAT_REQUEST = 1,
	HEARTBEAT_RESPONSE = 2,
};
#pragma once
#include "Utils/Imports/common.h"

enum ClientConnectType : uint8_t {
	CLIENT_CONNECT_BEGIN = 1,
	CLIENT_CONNECT_RELAY = 2,
	CLIENT_CONNECT_CONFIRM = 3,
};
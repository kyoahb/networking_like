#pragma once
#include "Utils/Imports/common.h"

enum class DisconnectResultType : uint8_t {
	UNKNOWN = 0,
	SUCCESS = 1,
	FORCED = 2,
	FAILED = 3
};

enum class DisconnectResultReason : uint8_t {
	UNKNOWN = 0,
	USER_REQUESTED = 1,
	SERVER_REQUESTED = 2,
	TIMEOUT = 3,
	ERROR = 4,
	SERVER_STOPPING = 5
};

class DisconnectResult {
public:
	DisconnectResultType type = DisconnectResultType::UNKNOWN; // Type of disconnect result
	DisconnectResultReason reason = DisconnectResultReason::UNKNOWN; // Reason for the disconnect result
	std::string message = ""; // Message associated with the disconnect result
	uint32_t time_taken = 0; // Time taken for the disconnect attempt (ms)
};

class PendingDisconnect {
public:
	DisconnectResultReason reason = DisconnectResultReason::UNKNOWN; // Reason for the disconnect result
	DisconnectResultType type = DisconnectResultType::UNKNOWN; // Type of disconnect result
	uint64_t request_time = 0; // Time when the disconnect request was made
};

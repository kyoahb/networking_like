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
	TIMEOUT = 2,
	ERROR = 3,
};

/*
 * @brief DisconnectResult class
 *
 * Represents the result of a disconnect request operation sent by the local NetworkUser.
 */
class DisconnectResult {
public:
	DisconnectResultType type = DisconnectResultType::UNKNOWN; // Type of disconnect result
	DisconnectResultReason reason = DisconnectResultReason::UNKNOWN; // Reason for the disconnect result
	std::string message = ""; // Message associated with the disconnect result
	uint32_t time_taken = 0; // Time taken for the disconnect attempt (ms)
};

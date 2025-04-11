#pragma once
#include "Utils/Imports/common.h"


// Class for server shutdown

enum class ShutdownResultType : uint8_t {
	UNKNOWN = 0,
	SUCCESS = 1,
	FAILURE = 2,
};

class ShutdownResult {
public:
	ShutdownResultType type = ShutdownResultType::UNKNOWN; // Type of shutdown result
	uint64_t time_taken = 0; // Time taken for the shutdown attempt (ms)
	std::string message = ""; // Message associated with the shutdown result

	uint8_t peers_graceful = 0; // Number of peers disconnected gracefully (not forced)
	uint8_t peers_forced = 0; // Number of peers forced to disconnect (not graceful)
};
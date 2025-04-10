#pragma once
#include "Utils/Imports/common.h"

enum class ConnectResultType : uint8_t {
	UNKNOWN = 0,
	SUCCESS = 1,
	FAILURE = 2,
	TIMEOUT = 3
};


class ConnectResult {
public:
	ConnectResultType type = ConnectResultType::UNKNOWN; // Type of connection result
	std::string message = ""; // Message associated with the connection result
	uint64_t time_taken = 0; // Time taken for the connection attempt (ms)
};
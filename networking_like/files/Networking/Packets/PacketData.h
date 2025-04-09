#pragma once
#include "Utils/General/SerializationUtils.h"
#include "Utils/Imports/common.h"

class PacketData {
public:
	virtual ~PacketData() = default;

	virtual std::string get_serialised() const; // Serialize the packet data to a string
	

};

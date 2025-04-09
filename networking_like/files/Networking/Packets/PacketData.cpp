#include "PacketData.h"

std::string PacketData::get_serialised() const {
	return SerializationUtils::serialize(*this);
}
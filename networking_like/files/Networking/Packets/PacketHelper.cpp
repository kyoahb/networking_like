#include "PacketHelper.h"

std::unordered_map<PacketDirection, std::string> PacketHelper::packet_direction_map = {
    {PacketDirection::UNKNOWN_DIRECTION, "UNKNOWN"},
    {PacketDirection::CLIENT_TO_SERVER, "CLIENT_TO_SERVER"},
    {PacketDirection::SERVER_TO_CLIENT, "SERVER_TO_CLIENT"},
	{PacketDirection::ANY_DIRECTION, "ANY"}
};

// Define the hierarchical structure of packet types and subtypes
std::unordered_map<PacketType, PacketHelper::TypeInfo> PacketHelper::packet_types = {
    {PacketType::UNKNOWN_TYPE, {"UNKNOWN", {}}},
    {PacketType::ANY_TYPE, {"ANY", {}}},

    {PacketType::CLIENT_CONNECT, {"CLIENT_CONNECT", {
        {255, {"ANY", {}}},
        {1, {"BEGIN", {}}},
        {2, {"RELAY", {}}},
        {3, {"CONFIRM", {}}},
    }}},

    {PacketType::CLIENT_DISCONNECT, {"CLIENT_DISCONNECT", {
        {255, {"ANY", {}}},
        {1, {"BEGIN", {}}},
        {2, {"RELAY", {}}},
        {3, {"CONFIRM", {}}},
    }}}
};

std::string PacketHelper::type_to_string(PacketType type) {
    auto it = packet_types.find(type);
    if (it != packet_types.end()) {
        return it->second.name;
    }
    return "UNKNOWN_TYPE";
}

std::string PacketHelper::subtype_to_string(PacketType type, uint8_t subtype) {
    auto type_it = packet_types.find(type);
    if (type_it != packet_types.end()) {
        auto subtype_it = type_it->second.subtypes.find(subtype);
        if (subtype_it != type_it->second.subtypes.end()) {
            return subtype_it->second.name;
        }
    }
    return "UNKNOWN_SUBTYPE";
}

std::string PacketHelper::direction_to_string(PacketDirection direction) {
    auto it = packet_direction_map.find(direction);
    if (it != packet_direction_map.end()) {
        return it->second;
    }
    return "UNKNOWN_DIRECTION";
}

std::string PacketHelper::types_to_string(PacketType type, uint8_t subtype) {
    return type_to_string(type) + ":" + subtype_to_string(type, subtype);
}

std::string PacketHelper::types_to_string(const Packet& packet) {
    return PacketHelper::types_to_string(packet.header.type, packet.header.subtype);
}
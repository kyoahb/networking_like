#pragma once
#include <unordered_map>
#include "Packet.h"


class PacketHelper {
public:
    // Define a structure to represent a type or subtype
    struct TypeInfo {
        std::string name;
        std::unordered_map<uint8_t, TypeInfo> subtypes;
    };

    // A single container for all type/subtype information
    static std::unordered_map<PacketType, TypeInfo> packet_types;
    static std::unordered_map<PacketDirection, std::string> packet_direction_map;

    // Main conversion methods
    static std::string type_to_string(PacketType type);
    static std::string subtype_to_string(PacketType type, uint8_t subtype);
    static std::string direction_to_string(PacketDirection direction);

    // Combined method
    static std::string types_to_string(PacketType type, uint8_t subtype);
    static std::string types_to_string(const Packet& packet);
};
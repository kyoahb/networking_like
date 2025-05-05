#pragma once
#include "Utils/Imports/common.h"
#include "Utils/General/TimeUtils.h"

enum PacketType : uint8_t {
	UNKNOWN_TYPE = 0,
	CHAT = 1,
	CLIENT_CONNECT = 2,
	ANY_TYPE = 255,
};

enum PacketDirection : uint8_t {
	UNKNOWN_DIRECTION = 0,
	CLIENT_TO_SERVER = 1,
	SERVER_TO_CLIENT = 2,
	ANY_DIRECTION = 255
};

/**
 * @brief Packet header structure
 *
 * This structure defines the header of a packet, which includes the type, direction,
 * subtype, size, and timestamp of the packet.
 */
struct PacketHeader {
	PacketType type = PacketType::UNKNOWN_TYPE; // Type of packet
	PacketDirection direction = PacketDirection::UNKNOWN_DIRECTION; // Direction of packet (client to server or server to client)
	uint8_t subtype = 0; // Subtype of packet (if applicable)
	uint32_t size = 0; // Size of the packet data
	uint64_t timestamp = 0; // Timestamp of when the packet was created
};

/**
 * @brief Packet structure
 *
 * This structure defines a packet, which includes the header and the data of the packet.
 */
class Packet {
public:
	PacketHeader header; // Header of the packet
	const char* data = nullptr; // Data of the packet
	bool is_valid = false; // Flag indicating if the packet is valid
	bool is_reliable = true; // Flag indicating if the packet is reliable
	
	Packet() = default;
	Packet(ENetPacket* packet);
	Packet(const PacketHeader& _header, const char* _data, bool _is_reliable = true);
	Packet(PacketType type, PacketDirection direction, uint8_t subtype, const char* _data, size_t size, bool _is_reliable = true);

	ENetPacket* to_enet_packet() const; // Converts the packet to an ENetPacket

};
#include "Packet.h"

Packet::Packet(ENetPacket* packet) {
	if (packet == nullptr || packet->data == nullptr || packet->dataLength < sizeof(PacketHeader)) {
		is_valid = false;
		Log::error("Invalid ENetPacket data, cannot create Packet.");
		return;
	}

	header = *reinterpret_cast<PacketHeader*>(packet->data);
	data = reinterpret_cast<const char*>(packet->data + sizeof(PacketHeader));
	is_valid = true;
	is_reliable = packet->flags & ENET_PACKET_FLAG_RELIABLE;
}

Packet::Packet(const PacketHeader& _header, const char* _data, bool _is_reliable) : header(_header), data(_data), is_reliable(_is_reliable) {
	if (_data == nullptr || _header.size == 0) {
		is_valid = false;
		Log::error("Invalid packet data or size, cannot create Packet.");
		return;
	}
	is_valid = true;
}

Packet::Packet(PacketType type, PacketDirection direction, uint8_t subtype, const char* _data, size_t size, bool _is_reliable) : is_reliable(_is_reliable) {
	header.type = type;
	header.direction = direction;
	header.subtype = subtype;
	header.size = static_cast<uint32_t>(size);
	header.timestamp = TimeUtils::get_current_time_millis();

	if (_data == nullptr || size == 0) {
		is_valid = false;
		Log::error("Invalid packet data or size, cannot create Packet.");
		return;
	}
	
	// Allocate memory for the packet data
	char* temp = new char[size];
	if (temp == nullptr) {
		is_valid = false;
		Log::error("Failed to allocate memory for packet data.");
		return;
	}

	// Copy the data into the allocated memory
	memcpy(temp, _data, size);

	this->data = temp;

	is_valid = true;
}

ENetPacket* Packet::to_enet_packet() const {
	if (!is_valid) {
		return nullptr;
	}
	size_t packet_size = sizeof(PacketHeader) + header.size;

	ENetPacket* packet = enet_packet_create(nullptr, packet_size, is_reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
	memcpy(packet->data, &header, sizeof(PacketHeader));
	memcpy(packet->data + sizeof(PacketHeader), data, header.size);
	packet->dataLength = packet_size;
	
	return packet;
}

bool Packet::is_event_packet(const ENetEvent event) {
	return event.type == ENET_EVENT_TYPE_RECEIVE &&
		event.peer &&
		event.packet &&
		event.packet->data &&
		event.packet->dataLength >= sizeof(PacketHeader);

}
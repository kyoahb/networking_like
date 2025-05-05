#pragma once

#include "Utils/Imports/common.h"
#include "Networking/Packets/Packet.h"
#include "Networking/Packets/PacketHelper.h"

class NetworkHelper {
public:
	// Not specifying anything, returns first event it finds
	static std::optional<ENetEvent> wait_for_event(ENetHost* host, unsigned int timeout_ms);

	// Specifying event type
	static std::optional<ENetEvent> wait_for_event(ENetHost* host, unsigned int timeout_ms, ENetEventType enet_type = ENET_EVENT_TYPE_RECEIVE);
	

	// Must be receive-event, specifying packet type
	static std::optional<ENetEvent> wait_for_event(ENetHost* host, unsigned int timeout_ms,
		PacketType packet_type = PacketType::ANY_TYPE, 
		PacketDirection packet_direction = PacketDirection::ANY_DIRECTION, 
		uint8_t packet_subtype = 255);
};
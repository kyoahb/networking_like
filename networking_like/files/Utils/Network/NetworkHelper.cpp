#include "NetworkHelper.h"

std::optional<ENetEvent> NetworkHelper::wait_for_event(ENetHost* host, unsigned int timeout_ms) {
	// Wait for an event with a specified timeout
	ENetEvent event;
	if (enet_host_service(host, &event, timeout_ms) > 0) {
		return event;
	}
	return std::nullopt;
}

std::optional<ENetEvent> NetworkHelper::wait_for_event(ENetHost* host, unsigned int timeout_ms, ENetEventType enet_type) {
	// Wait for an event of a specific type with a specified timeout
	ENetEvent event;

	unsigned int start_time = TimeUtils::get_current_time_millis();
	unsigned int remaining_time = timeout_ms;

	while (remaining_time > 0) {
		if (enet_host_service(host, &event, remaining_time) > 0) { // If <0, then an error has occured. Not tracking this yet
			if (event.type == enet_type) {
				return event;
			}
		}
		unsigned int elapsed_time = TimeUtils::get_time_since(start_time);
		remaining_time = timeout_ms - elapsed_time;
	}
	return std::nullopt;
}

std::optional<ENetEvent> NetworkHelper::wait_for_event(ENetHost* host, unsigned int timeout_ms, PacketType packet_type, PacketDirection packet_direction, uint8_t packet_subtype) {
	// Wait for a receive event with a specific packet type, direction, and subtype
	ENetEvent event;
	unsigned int start_time = TimeUtils::get_current_time_millis();
	unsigned int remaining_time = timeout_ms;

	while (remaining_time > 0) {
		if (enet_host_service(host, &event, remaining_time) > 0) {
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				Packet packet(event.packet);

				// ANY/255 is wildcard
				if ((packet.header.type == packet_type || packet_type == PacketType::ANY_TYPE) 
					&& (packet.header.direction == packet_direction || packet_direction == PacketDirection::ANY_DIRECTION) 
					&& (packet.header.subtype == packet_subtype || packet_subtype == 255 )) {
					return event;
				}
			}
		}
		unsigned int elapsed_time = TimeUtils::get_time_since(start_time);
		remaining_time = timeout_ms - elapsed_time;
	}
	return std::nullopt;
}
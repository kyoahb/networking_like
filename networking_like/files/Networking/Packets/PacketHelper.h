#pragma once
#include <unordered_map>
#include "Packet.h"


std::unordered_map<PacketType, std::string> packet_type_map = {
	{UNKNOWN_TYPE, "UNKNOWN"},
	{CHAT, "CHAT"},
};
std::unordered_map<PacketDirection, std::string> packet_direction_map = {
	{UNKNOWN_DIRECTION, "UNKNOWN"},
	{CLIENT_TO_SERVER, "CLIENT_TO_SERVER"},
	{SERVER_TO_CLIENT, "SERVER_TO_CLIENT"},
};
#pragma once
#include "Utils/Imports/common.h"

/*
 * @brief LocalNetPeer class
 *
 * Represents a local network peer (local server-side/client-side).
 */
class LocalNetPeer {
public:
	uint8_t id = 0;
	std::string handle = "LocalNetPeer";
	
	LocalNetPeer(uint8_t _id, std::string _handle);
	LocalNetPeer() = default;
};

/*
 * @brief NetPeer class
 *
 * Represents a network peer (server-side/client-side).
 */
class NetPeer : public LocalNetPeer {
public:
	ENetPeer* peer = nullptr;
	
	NetPeer(ENetPeer* peer, uint8_t _id, std::string _handle);
	NetPeer() = default;
};
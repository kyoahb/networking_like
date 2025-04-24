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

	bool operator==(const NetPeer& other) const {
		return this->peer == other.peer;
	}
};

namespace std {
	template<>
	struct hash<NetPeer> {
		std::size_t operator()(const NetPeer& np) const {
			// Hash based on the pointer address
			return std::hash<ENetPeer*>()(np.peer);
		}
	};
}
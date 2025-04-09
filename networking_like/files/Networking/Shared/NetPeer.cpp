#include "NetPeer.h"

LocalNetPeer::LocalNetPeer(uint8_t _id, std::string _handle = "") : id(_id), handle(_handle) {
	//
	if (_handle == "") {
		handle = "LocalPeer" + std::to_string(_id);
	}
}

NetPeer::NetPeer(ENetPeer* _peer, uint8_t _id, std::string _handle) : peer(_peer), LocalNetPeer(_id, _handle) {
	//
	Log::asserts(_peer != nullptr, "Cannot make NetPeer: Peer is nullptr");
	if (_handle == "") {
		handle = "Peer" + std::to_string(_id);
	}
}
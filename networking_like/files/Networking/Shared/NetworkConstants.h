#pragma once

namespace NetworkConstants {
	// Port number the server listens on
	constexpr const int Port = 8080;
	// Max simultaneous client connections allowed
	constexpr const int MaxConnections = 4;
	// Max channels per client connection (for message separation)
	constexpr const int MaxChannels = 2;
	// Bandwidth cap in bytes/sec (0 = unlimited)
	constexpr const int BandwidthLimit = 0;
	// ENet library timeout in ms (0 = no timeout)
	constexpr const int ENetTimeout = 0;
	// Sleep duration (ms) between server update cycles
	constexpr const int UpdateSleepMs = 10;
	// Interval (ms) to check for incoming packets
	constexpr const int PacketCheckInterval = 100;
	// Connection establishment/confirmation timeout in ms before disconnect
	constexpr const int ConnectionTimeout = 1000;


}
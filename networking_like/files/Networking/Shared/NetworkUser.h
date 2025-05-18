#pragma once
#include "Utils/Imports/common.h"
#include "Networking/Packets/Packet.h"
#include "Networking/Packets/PacketHelper.h"
#include "NetPeer.h"
#include <thread>
#include <atomic>
#include <future>


class NetworkUser {
public:
	NetworkUser();

	ENetAddress address = { 0 }; // Local address
	ENetHost* host = nullptr; // Local ENet host

	void start(); // Start network user (involves starting future for update loop)
	void stop(); // Stop network user (involves stopping future for update loop)

	bool send_packet(const Packet& packet, ENetPeer* peer); // Send a packet to a peer. Note ENetPeer is not const only as this function converts it to a NetPeer beforehand


protected:
	
	std::atomic<bool> active = false; // Whether update loop is active
	std::future<void> update_future; // Future for the update loop
	std::thread::id update_thread_id; // ID of the update thread
	const long UPDATE_INTERVAL = 1.0f; // Update interval in milliseconds

	virtual void update() = 0;
	void update_loop(); // Update loop for the network user
};
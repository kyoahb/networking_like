#pragma once
#include "Utils/Imports/common.h"
#include "Networking/Shared/NetworkUser.h"
#include "ClientPeerlist.h"
#include <future>

#include "Networking/Shared/FutureResults/ConnectResult.h"
#include "Networking/Shared/FutureResults/DisconnectResult.h"

#include "Networking/Packets/Data/ClientConnect.h"
#include "Networking/Packets/Data/ClientConnectBegin.h"
#include "Networking/Packets/Data/ClientConnectConfirm.h"

class Client : public NetworkUser {
public:
	ClientPeerlist peers;

	Client();
	~Client();

	bool is_connected();
	std::future<ConnectResult> connect(const std::string& ip, uint16_t port, const std::string& preferred_handle = "Client");
	std::future<DisconnectResult> disconnect();

	bool send_packet(const Packet& packet); // Send packet to server

private:
	void start(); // Begin packet update loop
	void stop(); // Stop packet update loop
	void update() override; // Update loop for the client

	ConnectResult connect_thread(const std::string& ip, uint16_t port, const std::string& preferred_handle);
	bool wait_for_connection_establishment(ENetPeer* server_peer);
	void send_connection_begin_packet(const std::string& preferred_handle);
	bool wait_for_connection_confirmation();
	
	DisconnectResult disconnect_thread(DisconnectResultReason reason);

	void receive_event(const ENetEvent& event);
	void disconnect_event(const ENetEvent& event);

};

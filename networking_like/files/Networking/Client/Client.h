#pragma once
#include "Utils/Imports/common.h"
#include "Networking/Shared/NetworkUser.h"
#include "ClientPeerlist.h"
#include <future>

#include "Networking/Shared/FutureResults/ConnectResult.h"
#include "Networking/Shared/FutureResults/DisconnectResult.h"

#include "Networking/Packets/Data/ClientConnect.h"
#include "Groups/CDisconnectGroup.h"
#include "Groups/CConnectGroup.h"

class Client : public NetworkUser, public std::enable_shared_from_this<Client> {
public:
	ClientPeerlist peers;

	Client();
	~Client();

	bool is_connected();
	std::future<ConnectResult> connect(const std::string& ip, uint16_t port, const std::string& preferred_handle = "Client");
	std::future<DisconnectResult> disconnect();
	void stop(); // Stop packet update loop

	bool send_packet(const Packet& packet); // Send packet to server
	void prepare_destroy(); // Prepare to destroy the client, stop the update loop and disconnect from server

private:
	void start(); // Begin packet update loop
	void update() override; // Update loop for the client


	std::vector<std::shared_ptr<CGroup>> groups = {}; // List of protocols
	std::shared_ptr<CConnectGroup> connect_group = nullptr; // Pointer to the connection protocol
	std::shared_ptr<CDisconnectGroup> disconnect_group = nullptr; // Pointer to the disconnection protocol

	void add_group(std::shared_ptr<CGroup> group);
	void initialize_groups();
	void destroy_groups();
	
	DisconnectResult disconnect_thread(DisconnectResultReason reason);

	void receive_event(const ENetEvent& event);
	void disconnect_event(const ENetEvent& event);

};

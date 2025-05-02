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

#include "Protocols/CConnect.h"

#include "Protocols/CProtocol.h"

class Client : public NetworkUser, public std::enable_shared_from_this<Client> {
public:
	ClientPeerlist peers;

	Client();
	~Client();

	void init();

	bool is_connected();
	std::future<ConnectResult> connect(const std::string& ip, uint16_t port, const std::string& preferred_handle = "Client");
	std::future<DisconnectResult> disconnect();

	bool send_packet(const Packet& packet); // Send packet to server

private:
	void start(); // Begin packet update loop
	void stop(); // Stop packet update loop
	void update() override; // Update loop for the client


	std::vector<std::shared_ptr<CProtocol>> protocols = {}; // List of protocols
	std::shared_ptr<CConnect> connect_protocol = nullptr; // Pointer to the connection protocol

	void add_protocol(std::shared_ptr<CProtocol> protocol);
	void initialize_protocols();
	void start_protocols();
	void stop_protocols();
	void update_protocols();
	void destroy_protocols();
	void dispatch_event_to_protocols(const ENetEvent& event);
	
	DisconnectResult disconnect_thread(DisconnectResultReason reason);

	void receive_event(const ENetEvent& event);
	void disconnect_event(const ENetEvent& event);

};

#pragma once
#include "Networking/Shared/NetworkUser.h"
#include "Networking/Shared/FutureResults/ShutdownResult.h"
#include "ServerPeerlist.h"
#include "Networking/Server/Events/SDisconnectGroup.h"
#include "Networking/Server/Events/SConnectGroup.h"
#include "Networking/Server/Events/SWorldGroup.h"
#include "Game/Events/EventList.h"
class Server : public NetworkUser, public std::enable_shared_from_this<Server> {
public:
	
	ServerPeerlist peers;

	Server(const std::string& _address, int _port);
	~Server();
	void prepare_destroy(); // Prepare to destroy the server, stop the update loop and disconnect from all clients
	void start();
	std::future<ShutdownResult> stop();

	std::future<DisconnectResult> disconnect_peer(ENetPeer* peer, DisconnectResultReason reason = DisconnectResultReason::SERVER_REQUESTED);
	bool send_packet(const Packet& packet, ENetPeer* peer);
	void broadcast_packet(const Packet& packet, std::vector<ENetPeer*> excluding = {}); // Broadcast a packet to all peers, excluding the specified peers

	std::shared_ptr<SWorldGroup> get_world_group();
private:
	const int MAX_CLIENTS = 32; // Maximum number of clients
	const int MAX_CHANNELS = 2; // Maximum number of channels
	const int BANDWIDTH_LIMIT = 0; // Bandwidth limit in bytes per second (0 = no limit)
	const int TIMEOUT_LIMIT = 0; // Timeout limit in milliseconds
	const int CHECK_INTERVAL = 1000; // Check interval in milliseconds

	std::string address_str;
	int port;

	// Protocols
	std::vector<std::shared_ptr<SGroup>> groups = {};
	std::shared_ptr<SDisconnectGroup> disconnect_group = nullptr;
	std::shared_ptr<SWorldGroup> world_group = nullptr;
	// Protocol management methods
	void add_group(std::shared_ptr<SGroup> group);
	void initialize_groups();
	void destroy_groups();


	// Other functions
	void update();

	void disconnect_event(const ENetEvent& event); // Client disconnects
	void connect_event(const ENetEvent& event); // New client connects
	void receive_event(const ENetEvent& event); // Receive event from ENet, pass to appropriate handler

	ShutdownResult stop_thread();

};
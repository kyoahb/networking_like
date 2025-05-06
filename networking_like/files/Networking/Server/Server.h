#pragma once
#include "Networking/Shared/NetworkUser.h"
#include "Networking/Shared/FutureResults/ShutdownResult.h"
#include "ServerPeerlist.h"
#include "Networking/Shared/FutureResults/DisconnectResult.h"
#include "Networking/Server/Protocols/SDisconnect.h"
#include "Networking/Server/Protocols/SConnect.h"
class Server : public NetworkUser, public std::enable_shared_from_this<Server> {
public:
	
	ServerPeerlist peers;

	Server(const std::string& _address, int _port);
	~Server();

	void start();
	std::future<ShutdownResult> stop();

	std::future<DisconnectResult> disconnect_peer(NetPeer& peer, DisconnectResultReason reason = DisconnectResultReason::SERVER_REQUESTED);
	bool send_packet(const Packet& packet, const NetPeer& peer);
	void broadcast_packet(const Packet& packet, std::vector<NetPeer> excluding = {}); // Broadcast a packet to all peers, excluding the specified peers
	void broadcast_packet(const Packet& packet, std::vector<ENetPeer*> excluding = {}); // Broadcast a packet to all peers, excluding the specified peers

private:
	const int MAX_CLIENTS = 32; // Maximum number of clients
	const int MAX_CHANNELS = 2; // Maximum number of channels
	const int BANDWIDTH_LIMIT = 0; // Bandwidth limit in bytes per second (0 = no limit)
	const int TIMEOUT_LIMIT = 0; // Timeout limit in milliseconds
	const int CHECK_INTERVAL = 1000; // Check interval in milliseconds

	std::string address_str;
	int port;

	// Protocols
	std::vector<std::shared_ptr<SProtocol>> protocols = {};
	std::shared_ptr<SDisconnect> disconnect_protocol = nullptr;
	// Protocol management methods
	void add_protocol(std::shared_ptr<SProtocol> protocol);
	void initialize_protocols();
	void start_protocols();
	void stop_protocols();
	void update_protocols();
	void destroy_protocols();

	// Protocol event handling
	void dispatch_event_to_protocols(const ENetEvent& event);


	// Other functions
	void update();

	void disconnect_event(const ENetEvent& event); // Client disconnects
	void connect_event(const ENetEvent& event); // New client connects
	void receive_event(const ENetEvent& event); // Receive event from ENet, pass to appropriate handler

	ShutdownResult stop_thread();

};
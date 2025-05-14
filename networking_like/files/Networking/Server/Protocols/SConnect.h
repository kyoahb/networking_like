#pragma once
#include "Networking/Server/Protocols/SProtocol.h"
#include <unordered_set>
#include "Networking/Packets/Data/ClientConnect.h"
#include "Networking/Packets/Data/ClientConnectBegin.h"
#include "Networking/Packets/Data/ClientConnectConfirm.h"
#include "Networking/Packets/Data/ClientConnectRelay.h"

/*
 * @brief SConnect class
 *
 * Protocol that manages client connections to the server.
 */

class SConnect : public SProtocol {
public:
	SConnect(std::shared_ptr<Server> _server);
	~SConnect();

	// unused overrides
	void on_start() override;
	void on_stop() override;
	void on_destroy() override;



	void on_update() override;
	void packet_event(const ENetEvent& event) override;
private:
	std::unordered_map<ENetPeer*, uint64_t> pending_begins; // Set of peers that are pending a CLIENT_CONNECT_BEGIN packet
	const unsigned int TIMEOUT_MS = 5000; // Timeout for client connection

	ClientConnectRelay netpeer_to_relay(const NetPeer& peer);
	std::vector<ClientConnectRelay> netpeer_list_to_relay_list(const std::vector<NetPeer>& peers, ENetPeer* exclude);

};

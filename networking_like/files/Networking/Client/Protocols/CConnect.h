#pragma once
#include "CProtocol.h"

#include "Utils/General/TimeUtils.h"
#include "Networking/Shared/FutureResults/ConnectResult.h"
#include "Utils/Network/NetworkHelper.h"
#include "Game/Events/EventList.h"
#include "Networking/Packets/Data/ClientDisconnectRelay.h"
class CConnect : public CProtocol {
public:
	explicit CConnect(std::shared_ptr<Client> _client);
	~CConnect() = default;

	void on_start() override;
	void on_stop() override;
	void on_update() override;
	void on_destroy() override;

	void packet_event(const ENetEvent& event) override;

	ConnectResult connect(const std::string& ip, uint16_t port, const std::string& preferred_handle);
	ENetPeer* begin_connection(const std::string& ip, uint16_t port, const std::string& preferred_handle);
	bool wait_for_connection_establishment(ENetPeer* server_peer);
	void send_connection_begin_packet(const std::string& preferred_handle);
	bool wait_for_connection_confirmation();


};

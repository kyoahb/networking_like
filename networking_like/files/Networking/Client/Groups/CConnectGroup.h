#pragma once
#include "Utils/Imports/common.h"
#include "CGroup.h"
#include "Utils/General/TimeUtils.h"
#include "Networking/Shared/FutureResults/ConnectResult.h"
#include "Utils/Network/NetworkHelper.h"
#include "Game/Events/EventList.h"
#include "Networking/Packets/Data/ClientConnect.h"

class CConnectGroup : public CGroup {
public:
    CConnectGroup(std::shared_ptr<Client> _client);
    ~CConnectGroup();

    void activate() override; // Group activated. In here, add event callbacks
    void deactivate() override; // Group deactivated. In here, remove event callbacks

    void on_event_receive(const Events::Client::EventReceiveData& data); // Event callback for receiving events

    ConnectResult connect(const std::string& ip, uint16_t port, const std::string& preferred_handle);
    ENetPeer* begin_connection(const std::string& ip, uint16_t port, const std::string& preferred_handle);
    bool wait_for_connection_establishment(ENetPeer* server_peer);
    void send_connection_begin_packet(const std::string& preferred_handle);
    bool wait_for_connection_confirmation();

private:
    int event_receive_callback = -1;
};

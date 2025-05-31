#include "CStateGroup.h"
#include "Networking/Client/Client.h"
#include "Networking/Packets/Data/ChangeState.h"

CStateGroup::CStateGroup(std::shared_ptr<Client> _client) : CGroup(_client) {
	activate();
}

CStateGroup::~CStateGroup() {
	deactivate();
}

void CStateGroup::activate() {
	update_callback = Events::Client::Update::register_callback([this](const Events::Client::UpdateData& data) { update(data); });
	event_receive_callback = Events::Client::EventReceive::register_callback([this](const Events::Client::EventReceiveData& data) { event_receive(data); });
	change_state_callback = Events::Game::StateChange::register_callback([this](const Events::Game::StateChangeData& data) { change_state(data); });
};

void CStateGroup::deactivate() {
	Events::Client::Update::unregister_callback(update_callback);
	Events::Client::EventReceive::unregister_callback(event_receive_callback);
	Events::Game::StateChange::unregister_callback(change_state_callback);

	client = nullptr;
};

void CStateGroup::event_receive(const Events::Client::EventReceiveData& data) {

}

void CStateGroup::update(const Events::Client::UpdateData& data) {

}

void CStateGroup::change_state(const Events::Game::StateChangeData& data) {
	// Send packet to server notifying them

	if (client != nullptr && !client->peers.is_connected()) return; // Cannot send when not connected

	ChangeStateNotification notification;
	notification.state_name = data.state_name;

	std::string serialised = SerializationUtils::serialize<ChangeStateNotification>(notification);
	Packet p(PacketType::CHANGE_STATE, PacketDirection::CLIENT_TO_SERVER, ChangeStateType::CHANGE_STATE_NOTIFICATION, serialised.data(), serialised.size());
	client->send_packet(p);	
}
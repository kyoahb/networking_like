#include "CWorldGroup.h"
#include "Networking/Client/Client.h"
CWorldGroup::CWorldGroup(std::shared_ptr<Client> _client) : CGroup(_client) {
	activate();
}

CWorldGroup::~CWorldGroup() {
	deactivate();
}

void CWorldGroup::activate() {
	update_callback = Events::Client::Update::register_callback([this](const Events::Client::UpdateData& data) { update(data); });
	event_receive_callback = Events::Client::EventReceive::register_callback([this](const Events::Client::EventReceiveData& data) { event_receive(data); });
};

void CWorldGroup::deactivate() {
	Events::Client::Update::unregister_callback(update_callback);
	Events::Client::EventReceive::unregister_callback(event_receive_callback);

	client = nullptr;
};

void CWorldGroup::event_receive(const Events::Client::EventReceiveData& data) {

}

void CWorldGroup::update(const Events::Client::UpdateData& data) {

}
#include "SStateGroup.h"
#include "Networking/Server/Server.h"
#include "Networking/Packets/Data/ChangeState.h"

SStateGroup::SStateGroup(std::shared_ptr<Server> _server) : SGroup(_server) {
	activate();
}

SStateGroup::~SStateGroup() {
	deactivate();
}

void SStateGroup::activate() {
	update_callback = Events::Server::Update::register_callback([this](const Events::Server::UpdateData& data) { update(data); });
	event_receive_callback = Events::Server::EventReceive::register_callback([this](const Events::Server::EventReceiveData& data) { event_receive(data); });
};

void SStateGroup::deactivate() {
	Events::Server::Update::unregister_callback(update_callback);
	Events::Server::EventReceive::unregister_callback(event_receive_callback);

	server = nullptr;
};

void SStateGroup::event_receive(const Events::Server::EventReceiveData& data) {
	LOG_SCOPE_SERVER_GROUP;
	if (!Packet::is_event_packet(data.event)) {
		return;
	}

	Packet p(data.event.packet);

	if (p.header.type != PacketType::CHANGE_STATE) return;

	// Get peer name
	std::string handle = server->peers.get_polite_handle(data.event.peer);

	if (p.header.subtype == ChangeStateType::CHANGE_STATE_NOTIFICATION) {
		// Deserialise packet
		ChangeStateNotification change_state = SerializationUtils::deserialize<ChangeStateNotification>(p.data, p.header.size);
		Log::trace("Received state change notification to " + change_state.state_name + " from " + handle);

	}
}

void SStateGroup::update(const Events::Server::UpdateData& data) {

}
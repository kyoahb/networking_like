#include "SWorldGroup.h"
#include "Networking/Server/Server.h"
SWorldGroup::SWorldGroup(std::shared_ptr<Server> _server) : SGroup(_server) {
	activate();
}

SWorldGroup::~SWorldGroup() {
	deactivate();
}

void SWorldGroup::activate() {
	update_callback = Events::Server::Update::register_callback([this](const Events::Server::UpdateData& data) { update(data); });
	event_receive_callback = Events::Server::EventReceive::register_callback([this](const Events::Server::EventReceiveData& data) { event_receive(data); });
};

void SWorldGroup::deactivate() {
	Events::Server::Update::unregister_callback(update_callback);
	Events::Server::EventReceive::unregister_callback(event_receive_callback);

	server = nullptr;
};

void SWorldGroup::event_receive(const Events::Server::EventReceiveData& data) {

}

void SWorldGroup::update(const Events::Server::UpdateData& data) {

}

void SWorldGroup::generate_world() {
	// This should be setup with an actual world generation process
	// For now, a placeholder will be used
	world = WorldObject();
	world.counter = 0;
}

void SWorldGroup::send_delta_world() {
	world.counter += 1;

	// Send new world
}

void SWorldGroup::send_initial_world() {
	// Send world

	send_world();

}

void SWorldGroup::send_world() {
	WorldFull winfo;
	winfo.world = world;

	std::string serialised = SerializationUtils::serialize<WorldFull>(winfo);

	Packet p(PacketType::WORLD_INFO, PacketDirection::SERVER_TO_CLIENT, WorldInfo::WORLD_FULL, serialised.data(), serialised.size());
	server->broadcast_packet(p);
}
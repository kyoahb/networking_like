#include "WorldState.h"
#include "Game/Game.h"
WorldState::WorldState(Game& _game) : GameState(_game, "WorldState") {

}

WorldState::~WorldState() {
	// Destructor implementation
}

void WorldState::on_draw() {
	// Drawing logic for the world state

	//ImGui::Text("Game World Placeholder Text");
	
	if (player)
	{
		BeginMode3D(player->get_camera());
		player->update();
		player->draw();

		raylib::Vector3 position = raylib::Vector3( 0.0f, 0.0f, 0.0f );
		position.DrawCube(raylib::Vector3( 1.0f, 1.0f, 1.0f ), RED);
	
		EndMode3D();
	}
}

void WorldState::on_activate() {
	// Logic to execute when the world state is activated

	// Setup callbacks
	update_callback = Events::Client::Update::register_callback([this](const Events::Client::UpdateData& data) { update(data); });
	event_receive_callback = Events::Client::EventReceive::register_callback([this](const Events::Client::EventReceiveData& data) { event_receive(data); });


	if (game.is_local_server()) {
		// We are going to tell the server to generate the world and send it to peers
		auto world_group = game.server->get_world_group();

		world_group->generate_world();
		world_group->send_initial_world();
	}

	// Just as a test 
	player = std::make_shared<LocalPlayer>();
	Log::trace("test");
}

void WorldState::on_deactivate() {
	// Logic to execute when the world state is deactivated

	// Destroy callbacks
	Events::Client::Update::unregister_callback(update_callback);
	Events::Client::EventReceive::unregister_callback(event_receive_callback);
}

void WorldState::event_receive(const Events::Client::EventReceiveData& data) {
	LOG_SCOPE_GAME;
	if (!Packet::is_event_packet(data.event)) return;

	Packet p(data.event.packet);
	if (p.header.type == PacketType::WORLD_INFO && p.header.subtype == WorldInfo::WORLD_FULL) {
		// Deserialise
		Log::trace("Received a valid WORLD_FULL Packet, saving it as world.");
		WorldFull winfo = SerializationUtils::deserialize<WorldFull>(p.data, p.header.size);
		world = winfo.world;
	}

}

void WorldState::update(const Events::Client::UpdateData& data) {

}
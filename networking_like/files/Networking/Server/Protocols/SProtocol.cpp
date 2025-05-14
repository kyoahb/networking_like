#include "SProtocol.h"

SProtocol::SProtocol(std::shared_ptr<Server> _server, std::string _name) : Protocol(_name), server(_server){
	LOG_SCOPE_SERVER_PROTOCOL;
	Log::trace(get_name() + " server_protocol constructor called");
};

SProtocol::~SProtocol() {
	LOG_SCOPE_SERVER_PROTOCOL;
	on_destroy();
	cleanup();
};


void SProtocol::packet_event(const ENetEvent& event) {
};

void SProtocol::on_destroy() {};
void SProtocol::on_start() {};
void SProtocol::on_stop() {};
void SProtocol::on_update() {};

void SProtocol::start() {
	Log::asserts(server != nullptr, "Server is null in SProtocol::start()");
	on_start();
};

void SProtocol::stop() {
	Log::asserts(server != nullptr, "Server is null in SProtocol::stop()");
	on_stop();
};

void SProtocol::update() {
	Log::asserts(server != nullptr, "Server is null in SProtocol::update()");
	on_update();
};

void SProtocol::destroy() {
	Log::asserts(server != nullptr, "Server is null in SProtocol::destroy()");
	on_destroy();
};

void SProtocol::cleanup() {
	server = nullptr; // Set server to nullptr
}
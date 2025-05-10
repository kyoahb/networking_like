#include "CProtocol.h"


CProtocol::CProtocol(std::shared_ptr<Client> _client, std::string _name) : Protocol(_name), client(_client) {
	LOG_SCOPE_CLIENT_PROTOCOL;
	Log::trace(get_name() + " client_protocol constructor called");
};

CProtocol::~CProtocol() {
	LOG_SCOPE_CLIENT_PROTOCOL;
	on_destroy();
	cleanup();
};

void CProtocol::on_destroy() {};
void CProtocol::on_start() {};
void CProtocol::on_stop() {};
void CProtocol::on_update() {};

void CProtocol::cleanup() {
	client = nullptr; // Set client to nullptr

}

void CProtocol::start() {
	Log::asserts(client != nullptr, "Client is null in CProtocol::start()");
	on_start();
};

void CProtocol::stop() {
	Log::asserts(client != nullptr, "Client is null in CProtocol::stop()");
	on_stop();
};

void CProtocol::update() {
	Log::asserts(client != nullptr, "Client is null in CProtocol::update()");
	on_update();
};

void CProtocol::destroy() {
	Log::asserts(client != nullptr, "Client is null in CProtocol::destroy()");
	on_destroy();
};
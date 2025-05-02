#include "CProtocol.h"


CProtocol::CProtocol(std::shared_ptr<Client> _client, std::string _name) : Protocol(_name), client(_client) {
	LOG_SCOPE_CLIENT_PROTOCOL;
	Log::trace(get_name() + " client_protocol constructor called");
};
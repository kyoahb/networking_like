#include "SProtocol.h"

SProtocol::SProtocol(std::shared_ptr<Server> _server, std::string _name) : Protocol(_name), server(_server){
	LOG_SCOPE_SERVER_PROTOCOL;
	Log::trace(get_name() + " server_protocol constructor called");
};

#include "SProtocol.h"

SProtocol::SProtocol(std::shared_ptr<Server> _server, std::string _name) : name(_name), server(_server){
	LOG_SCOPE_SERVER_PROTOCOL;
	Log::trace(get_name() + " constructor called");
};

const std::string& SProtocol::get_name() const {
	return name;
}
#include "SProtocol.h"

SProtocol::SProtocol(std::string _name) {
	LOG_SCOPE_SERVER_PROTOCOL;
	name = _name;
	Log::trace(get_name() + " constructor called");
};

const std::string& SProtocol::get_name() const {
	return name;
}

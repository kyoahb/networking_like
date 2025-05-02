#include "Protocol.h"

Protocol::Protocol(const std::string& _name) : name(_name) {
}

const std::string& Protocol::get_name() const {
	return name;
}
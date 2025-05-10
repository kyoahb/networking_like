#include "Protocol.h"

Protocol::Protocol(const std::string& _name) : name(_name) {
}

const std::string& Protocol::get_name() const {
	return name;
}

void Protocol::start() {
	on_start();
}
void Protocol::stop() {
	on_stop();
}
void Protocol::update() {
	on_update();
}
void Protocol::destroy() {
	on_destroy();
}
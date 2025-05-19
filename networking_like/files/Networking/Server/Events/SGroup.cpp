#include "SGroup.h"


SGroup::SGroup(std::shared_ptr<Server> _server) : server(_server) {
}

SGroup::~SGroup() {
	// Destructor implementation
}

void SGroup::activate() {
	// Add event callbacks
}

void SGroup::deactivate() {
	// Remove event callbacks
	server = nullptr;
}
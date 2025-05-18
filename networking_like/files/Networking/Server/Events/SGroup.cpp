#include "SGroup.h"


SGroup::SGroup(std::shared_ptr<Server> _server) : server(_server) {
	activate();
}

SGroup::~SGroup() {
	// Destructor implementation
	deactivate();
}

void SGroup::activate() {
	// Add event callbacks
}

void SGroup::deactivate() {
	// Remove event callbacks
	server = nullptr;
}
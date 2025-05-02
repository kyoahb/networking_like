#pragma once
#include "Utils/Imports/common.h"

class Protocol {
public:
	explicit Protocol(const std::string& _name);

	virtual void start() = 0; // Called on start()
	virtual void stop() = 0; // Called on stop()
	virtual void update() = 0; // Called on update()
	virtual void destroy() = 0; // Called on destructor

	const std::string& get_name() const;

protected:
	std::string name = "Unnamed Protocol"; // Name of the protocol

};
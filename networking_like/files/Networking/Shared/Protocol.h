#pragma once
#include "Utils/Imports/common.h"

class Protocol {
public:
	explicit Protocol(const std::string& _name);

	// Should not be modified in derived protocol
	virtual void start(); // Called on start()
	virtual void stop(); // Called on stop()
	virtual void update(); // Called on update()
	virtual void destroy(); // Called on destructor

	// Should be modified in derived protocol.
	virtual void on_stop() = 0;
	virtual void on_start() = 0;
	virtual void on_update() = 0;
	virtual void on_destroy() = 0;

	const std::string& get_name() const;

protected:
	std::string name = "Unnamed Protocol"; // Name of the protocol

};
#pragma once
#include "Entity.h"
#include "Networking/Shared/NetPeer.h"

class Player : public Entity {
public:
	Player(const NetPeer& peer);
	Player(const TransmittablePlayer& p); 
	Player(); // Default constructor
	~Player();

	// Local variables
	const raylib::Camera& get_camera();

	void update(); // Updates camera + position based on mouse / keyboard input
	void draw(); // Draws model in current position with rotation


	TransmittablePlayer to_transmittable() const;

private:
	int peer_id = -1; // Corresponds to a peer.

	raylib::Camera camera;
	raylib::Vector3 camera_offset = raylib::Vector3(0.0f, 2.0f, 0.0f); // Distance of camera from player centre

	float move_speed = 10.0f;
	float look_sensitivity = 0.05f;

	void setup_camera(); // Sets up camera with default values
	void move_directionally(float amount); // Moves amount in direction player is facing
	
};
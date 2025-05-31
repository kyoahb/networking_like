#pragma once
#include "Entity.h"

class LocalPlayer : public Entity {
public:
	LocalPlayer();
	~LocalPlayer();

	// Local variables
	const raylib::Camera& get_camera();

	void update(); // Updates camera + position based on mouse / keyboard input
	void draw(); // Draws model in current position with rotation

private:
	raylib::Camera camera;
	raylib::Vector3 camera_offset = raylib::Vector3(0.0f, 2.0f, 0.0f); // Distance of camera from player centre

	float move_speed = 10.0f;
	float look_sensitivity = 0.05f;

	void move_directionally(float amount); // Moves amount in direction player is facing

};
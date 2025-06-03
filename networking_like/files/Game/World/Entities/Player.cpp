#include "Player.h"

Player::Player(const NetPeer& peer) : Entity(), peer_id(peer.id) {

	// Setup camera
	camera = { 0 };
	camera.position = raylib::Vector3(position.x + camera_offset.x, position.y + camera_offset.y, position.z + camera_offset.z);    // Camera position
	camera.target = raylib::Vector3{ 0.0f, 2.0f, 0.0f };      // Camera looking at point
	camera.up = raylib::Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 90.0f;                              // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;           // Camera projection type
}

Player::~Player() {

}

// Locals
const raylib::Camera& Player::get_camera() {
	return camera;
}

// Methods

void Player::draw() {

	Entity::draw(); // No need for anything different
}

void Player::update() {

	// Update camera
	raylib::Vector3 rotation_delta = { GetMouseDelta().y, GetMouseDelta().x, 0.0f };
	rotation_delta *= look_sensitivity; // Scale rotation delta
	rotate(rotation_delta);
	rotation.x = MathUtils::clamp(rotation.x, -89.0f, 89.0f); // Clamp up/down rotation

	// Update player + camera position
	move_directionally(move_speed * GetFrameTime()); // Move proportionally to deltaTime
	camera.SetPosition(position + camera_offset);

	// Set camera target
	camera.target = MathUtils::calculate_target(camera.position, rotation, 1.0f);
}

void Player::move_directionally(float amount) {
	int forward_amt = IsKeyDown(KEY_W) - IsKeyDown(KEY_S);
	int right_amt = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);

	raylib::Vector3 forward = MathUtils::forward(rotation);
	raylib::Vector3 right = MathUtils::right(rotation);

	raylib::Vector3 position_delta = { 0.0f, 0.0f, 0.0f };
	position_delta += forward * forward_amt;
	position_delta += right * right_amt;

	position_delta.y = 0.0f; // Prevent vertical movement


	// Normalize movement to prevent diagonal speed boost
	if (position_delta.Length() > 1.0f) {
		position_delta.Normalize();
	}

	move(position_delta * amount);
}
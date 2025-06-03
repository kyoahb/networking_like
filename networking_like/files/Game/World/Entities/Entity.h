#pragma once

#include "Utils/Imports/common.h"
#include "Utils/General/MathUtils.h"

// We must ensure Entities are ONLY (!) made as shared_ptrs
// Or exist as shared_ptrs before children are added to them
class Entity {
public:
	Entity(
		raylib::Vector3 _position = raylib::Vector3(0.0f, 0.0f, 0.0f),
		raylib::Vector3 _rotation = raylib::Vector3(0.0f, 0.0f, 0.0f),
		raylib::Vector3 _scale = raylib::Vector3(1.0f, 1.0f, 1.0f)
	);
	Entity(
		raylib::Model _model,
		raylib::Vector3 _position = raylib::Vector3(0.0f, 0.0f, 0.0f),
		raylib::Vector3 _rotation = raylib::Vector3(0.0f, 0.0f, 0.0f),
		raylib::Vector3 _scale = raylib::Vector3(1.0f, 1.0f, 1.0f)
	);
	~Entity();

	// Local variables
	const raylib::Vector3 get_position() const;
	void set_position(raylib::Vector3 _position);
	void move(raylib::Vector3 delta);

	const raylib::Vector3 get_rotation() const;
	void set_rotation(raylib::Vector3 _rotation);
	void rotate(raylib::Vector3 delta);

	const raylib::Vector3 get_scale() const;
	void set_scale(raylib::Vector3 _scale);
	void mult_scale(raylib::Vector3 multiplier);

	const raylib::Model& get_model() const;
	void set_model(raylib::Model& _model);

	unsigned int get_id() const;

	unsigned int get_parent_id() const;
	std::vector<std::shared_ptr<Entity>> get_children() const;
	void add_child(std::shared_ptr<Entity> child);
	void set_parent_id(unsigned int _parent_id);
	
	// Methods

	void draw() const; // ASSUME CAMERA IS BOUND!


protected:
	static unsigned int next_id; // STATIC MEMBER OF Entity.

	unsigned int ID; // Distinct entity ID of THIS object

	raylib::Vector3 position;
	raylib::Vector3 rotation;
	raylib::Vector3 scale;
	raylib::Model model;


	// Children of an object always stay in same relative position IF parent object is moved linearly
	// Children will NOT stay in a relative position IF
	//					- Parent object is moved with a setter
	//					- Child object is moved independently
	std::vector<std::shared_ptr<Entity>> children;
	unsigned int parent_id = 0; // Not all objects will have parents, must check if ==0
};

#include "Entity.h"

unsigned int Entity::next_id = 0;

Entity::Entity(
    raylib::Vector3 _position,
    raylib::Vector3 _rotation,
    raylib::Vector3 _scale
) : position(_position), rotation(_rotation), scale(_scale)
{
    model = LoadModel("files/Assets/cube.obj");

    ID = next_id++;
}

Entity::Entity(
    raylib::Model _model, 
    raylib::Vector3 _position, 
    raylib::Vector3 _rotation, 
    raylib::Vector3 _scale) 
    : position(_position), rotation(_rotation), scale(_scale)
{
    if (!_model.IsValid())
        model = LoadModel("files/Assets/cube.obj");
    ID = next_id++;
}

Entity::~Entity() {

}

// Local variable methods

// position
const raylib::Vector3 Entity::get_position() const {
    return position;
}
void Entity::set_position(raylib::Vector3 _position) {
    position = _position;
}
void Entity::move(raylib::Vector3 delta) {
    position += delta;
}

// rotation
const raylib::Vector3 Entity::get_rotation() const {
    return rotation;
}
void Entity::set_rotation(raylib::Vector3 _rotation) {
    rotation = _rotation;
}
void Entity::rotate(raylib::Vector3 delta) {
    rotation += delta;
}

// scale
const raylib::Vector3 Entity::get_scale() const {
    return scale;
}
void Entity::set_scale(raylib::Vector3 _scale) {
    scale = _scale;
}
void Entity::mult_scale(raylib::Vector3 multiplier) {
    scale *= multiplier;
}

// model
const raylib::Model& Entity::get_model() const {
    return model;
}
void Entity::set_model(raylib::Model& _model) {
    model = std::move(_model);
}

// id
unsigned int Entity::get_id() const {
    return ID;
}

// children
std::weak_ptr<Entity> Entity::get_parent() const {
    return parent;
}

std::vector<std::shared_ptr<Entity>> Entity::get_children() const {
    return children;
}

void Entity::add_child(std::shared_ptr<Entity> child) {
    if (child->get_id() == get_id()) {
        Log::warn("Entity tried to append itself as its own child.");
        return;
    }
    auto child_parent = child->get_parent().lock();
    if (child_parent && child_parent->get_id() != get_id()) {
        Log::warn("Entity tried to append an existing child of itself as a new child.");
        return;
    }

    // Ensure this entity is managed by a shared_ptr
    try {
        std::shared_ptr<Entity> parent = shared_from_this();
        child->set_parent(parent);
        children.push_back(child);
    }
    catch (const std::bad_weak_ptr&) {
        Log::asserts(false, "Entity must be managed by a shared_ptr before adding children!");
    }
}

void Entity::set_parent(std::shared_ptr<Entity> _parent) {
    parent = _parent;
}

// Methods

// draw
void Entity::draw() const {
    raylib::Vector4 axis_angle_repr = MathUtils::euler_to_axis_angle(rotation);
    model.Draw(position, { axis_angle_repr.x, axis_angle_repr.y, axis_angle_repr.z }, axis_angle_repr.w, scale);
}
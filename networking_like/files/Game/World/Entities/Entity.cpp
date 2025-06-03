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

Entity::Entity(const TransmittableEntity& e) : position(e.position), rotation(e.rotation), scale(e.scale) {
    ID = e.id; 
    next_id++; // Just for consistency
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
void Entity::mult_scale(float multiplier) {
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
unsigned int Entity::get_parent_id() const {
    return parent_id; // Remember if parent_id==0 then it does not have a parent
}

std::vector<std::shared_ptr<Entity>> Entity::get_children() const {
    return children;
}

void Entity::add_child(std::shared_ptr<Entity> child) {
    if (child->get_id() == get_id()) {
        Log::warn("Entity tried to append itself as its own child.");
        return;
    }
    unsigned int child_parentid = child->get_parent_id();
    if (child_parentid != 0 && child_parentid != get_id()) {
        Log::warn("Entity tried to append an existing child of itself as a new child.");
        return;
    }

    // Ensure this entity is managed by a shared_ptr
    try {
        child->set_parent_id(child_parentid);
        children.push_back(child);
    }
    catch (const std::bad_weak_ptr&) {
        Log::asserts(false, "Entity must be managed by a shared_ptr before adding children!");
    }
}

void Entity::set_parent_id(unsigned int _parent_id) {
    parent_id = _parent_id;
}

// Methods

// draw
void Entity::draw() const {
    raylib::Vector4 axis_angle_repr = MathUtils::euler_to_axis_angle(rotation);
    model.Draw(position, { axis_angle_repr.x, axis_angle_repr.y, axis_angle_repr.z }, axis_angle_repr.w, scale);
}

// to transmittable

std::vector<unsigned int> Entity::get_children_ids() const {
    std::vector<unsigned int> ids = {};
    for (const auto& child : children) {
        if (child) {
            ids.push_back(child->get_id());
        }
    }
}

TransmittableEntity Entity::to_transmittable() const {
    TransmittableEntity e;
    e.id = ID;
    e.parent_id = parent_id;
    e.children_ids = get_children_ids();
    e.position = position;
    e.rotation = rotation;
    e.scale = scale;
    
    return e;
}
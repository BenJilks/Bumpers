#include "collider.hpp"
#include "engine/physics/collision_shape.hpp"
using namespace Engine;
using namespace Object;

Collider::Collider(std::shared_ptr<CollisionShape> shape)
    : m_shape(shape)
{
}


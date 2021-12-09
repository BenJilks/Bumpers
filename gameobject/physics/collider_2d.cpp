#include "collider_2d.hpp"
using namespace Engine;
using namespace Object;

Collider2D::Collider2D(std::shared_ptr<CollisionShape2D> shape)
    : m_shape(shape)
{
}


#include "broad_phase_collision.hpp"
#include "collision_shape.hpp"
#include "collision_shape_utils.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/physics/collider.hpp"
#include "gameobject/physics/physics_body.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <numbers>
using namespace Engine;
using namespace Object;
using namespace glm;

static std::pair<vec2, vec2> calculate_bounding_box(CollisionShape &shape, const Transform::Computed &transform)
{
    switch (shape.type())
    {
        case CollisionShape::Type::AABB:
        {
            auto aabb = static_cast<const CollisionShapeAABB&>(shape);
            auto center = aabb.center() + vec_3to2(transform.position);
            auto half_widths = aabb.half_widths() * vec_3to2(transform.scale);
            return std::make_pair(center, half_widths);
        }
        
        case CollisionShape::Type::Circle:
        {
            auto circle = static_cast<const CollisionShapeCircle&>(shape);
            auto center = circle.center() + vec_3to2(transform.position);
            auto half_widths = circle.radius() * transform.scale;
            return std::make_pair(center, half_widths);
        }

        case CollisionShape::Type::OBB:
        {
            auto obb = static_cast<const CollisionShapeOBB&>(shape);
            auto center = obb.center() + vec_3to2(transform.position);
            // NOTE: Not accurate but fast enough to make up for it, without caching.
            auto half_widths = obb.half_widths() * std::max(transform.scale.x, transform.scale.y) * (float)std::numbers::sqrt2;
            return std::make_pair(center, glm::abs(half_widths));
        }

        case CollisionShape::Type::ConvexPolygon:
        {
            auto &convex_polygon = static_cast<CollisionShapeConvexPolygon&>(shape);
            auto bounding_box = convex_polygon.bounding_box(transform.scale, transform.rotation.y);
            auto center = bounding_box.first + vec_3to2(transform.position);
            auto half_widths = bounding_box.second;
            return std::make_pair(center, half_widths);
        }
    }
    
    assert (false);
}

static bool are_bouding_boxes_colliding(std::pair<vec2, vec2> lhs, std::pair<vec2, vec2> rhs)
{
    auto lhs_center = lhs.first;
    auto lhs_half_widths = lhs.second;
    auto rhs_center = rhs.first;
    auto rhs_half_widths = rhs.second;
    
    if (abs(lhs_center.x - rhs_center.x) > (lhs_half_widths.x + rhs_half_widths.x) || 
        abs(lhs_center.y - rhs_center.y) > (lhs_half_widths.y + rhs_half_widths.y))
    {
        return false;
    }
    return true;
}

static void check_all_colliders(CollisionResolver::CollisionObject& lhs, CollisionResolver::CollisionObject &rhs,
    std::function<void(CollisionResolver::CollisionObject&, Object::Collider&, CollisionResolver::CollisionObject&, Object::Collider&)> callback)
{
    for (auto *lhs_collider : lhs.object.get<Collider>())
    {
        for (auto *rhs_collider : rhs.object.get<Collider>())
        {
            auto lhs_bounding_box = calculate_bounding_box(lhs_collider->shape(), lhs.transform.computed_transform());
            auto rhs_bounding_box = calculate_bounding_box(rhs_collider->shape(), rhs.transform.computed_transform());
            
            if (are_bouding_boxes_colliding(lhs_bounding_box, rhs_bounding_box))
                callback(lhs, *lhs_collider, rhs, *rhs_collider);
        }
    }
}

void BroadPhaseCollision::for_each_narrow_phase_pair(
    std::vector<CollisionResolver::CollisionObject> &collition_objects,
    std::function<void(CollisionResolver::CollisionObject&, Object::Collider&, CollisionResolver::CollisionObject&, Object::Collider&)> callback)
{
    for (auto &lhs : collition_objects)
    {
        for (auto &rhs : collition_objects)
        {
            if (&lhs == &rhs)
                continue;
            if ((lhs.body == nullptr || lhs.body->is_static()) && (rhs.body == nullptr || rhs.body->is_static()))
                continue;
            check_all_colliders(lhs, rhs, callback);
        }
    }
}


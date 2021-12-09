#include "broad_phase_collision_2d.hpp"
#include "collision_shape_2d.hpp"
#include "collision_shape_utils_2d.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/physics/collider_2d.hpp"
#include "gameobject/physics/physics_body_2d.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <numbers>
using namespace Engine;
using namespace Object;
using namespace glm;

static std::pair<vec2, vec2> calculate_bounding_box(CollisionShape2D &shape, const Transform::Computed2D &transform)
{
    switch (shape.type())
    {
        case CollisionShape2D::Type::AABB:
        {
            auto aabb = static_cast<const CollisionShapeAABB2D&>(shape);
            auto center = transform_by(aabb.center(), transform);
            auto half_widths = aabb.half_widths() * transform.scale;
            return std::make_pair(center, half_widths);
        }
        
        case CollisionShape2D::Type::Circle:
        {
            auto circle = static_cast<const CollisionShapeCircle2D&>(shape);
            auto center = transform_by(circle.center(), transform);
            auto half_widths = circle.radius() * transform.scale;
            return std::make_pair(center, half_widths);
        }

        case CollisionShape2D::Type::OBB:
        {
            auto obb = static_cast<const CollisionShapeOBB2D&>(shape);
            auto center = transform_by(obb.center(), transform);
            // NOTE: Not accurate but fast enough to make up for it, without caching.
            auto scaled_obb_half_widths = obb.half_widths() * transform.scale;
            auto max_obb_width = std::max(scaled_obb_half_widths.x, scaled_obb_half_widths.y);
            auto half_widths = vec2(max_obb_width * (float)std::numbers::sqrt2);
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

static void check_all_colliders(CollisionResolver2D::CollisionObject& lhs, CollisionResolver2D::CollisionObject &rhs,
    std::function<void(CollisionResolver2D::CollisionObject&, Object::Collider2D&, CollisionResolver2D::CollisionObject&, Object::Collider2D&)> callback)
{
    for (auto *lhs_collider : lhs.object.get<Collider2D>())
    {
        for (auto *rhs_collider : rhs.object.get<Collider2D>())
        {
            auto lhs_bounding_box = calculate_bounding_box(lhs_collider->shape(), lhs.transform.computed_transform_2d());
            auto rhs_bounding_box = calculate_bounding_box(rhs_collider->shape(), rhs.transform.computed_transform_2d());
            
            if (are_bouding_boxes_colliding(lhs_bounding_box, rhs_bounding_box))
                callback(lhs, *lhs_collider, rhs, *rhs_collider);
        }
    }
}

void BroadPhaseCollision2D::for_each_narrow_phase_pair(
    std::vector<CollisionResolver2D::CollisionObject> &collition_objects,
    std::function<void(CollisionResolver2D::CollisionObject&, Object::Collider2D&, CollisionResolver2D::CollisionObject&, Object::Collider2D&)> callback)
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


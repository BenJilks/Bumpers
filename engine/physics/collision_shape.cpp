#include "collision_shape.hpp"
#include "collision_shape_utils.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/physics/collider.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/dual_quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/matrix.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <numbers>
using namespace Engine;
using namespace Object;
using namespace glm;

CollisionShape::~CollisionShape()
{
}

static CollisionShape::CollisionResult collide_aabb_aabb(
    const CollisionShape &lhs_shape, const Transform::Computed2D &lhs_transform, 
    const CollisionShape &rhs_shape, const Transform::Computed2D &rhs_transform)
{
    auto &lhs_aabb = static_cast<const CollisionShapeAABB&>(lhs_shape);
    auto &rhs_aabb = static_cast<const CollisionShapeAABB&>(rhs_shape);

    auto lhs_center = transform_by(lhs_aabb.center(), lhs_transform);
    auto lhs_half_widths = lhs_aabb.half_widths() * lhs_transform.scale;
    auto rhs_center = transform_by(rhs_aabb.center(), rhs_transform);
    auto rhs_half_widths = rhs_aabb.half_widths() * rhs_transform.scale;

    auto penetration_x = abs(lhs_center.x - rhs_center.x) - (lhs_half_widths.x + rhs_half_widths.x);
    if (penetration_x > 0) return CollisionShape::CollisionResult {};
    
    auto penetration_y = abs(lhs_center.y - rhs_center.y) - (lhs_half_widths.y + rhs_half_widths.y);
    if (penetration_y > 0) return CollisionShape::CollisionResult {};

    vec2 normal(0, 0);
    if (penetration_x < penetration_y)
        normal = vec2(0, lhs_center.y < rhs_center.y ? -1 : 1);
    if (penetration_x > penetration_y)
        normal = vec2(lhs_center.x < rhs_center.x ? -1 : 1, 0);

    auto penetration = abs(penetration_x * normal.x + penetration_y * normal.y);
    return CollisionShape::CollisionResult 
    {
        .is_colliding = true,
        .penetration_distance = penetration,
        .normal = normal,
        .intersection_points = { rhs_center + normal * rhs_half_widths },
        .intersection_point_count = 1,
    };
}

static CollisionShape::CollisionResult collide_circle_circle(
    const CollisionShape& lhs_shape, const Transform::Computed2D& lhs_transform,
    const CollisionShape& rhs_shape, const Transform::Computed2D& rhs_transform)
{
    auto& lhs_circle = static_cast<const CollisionShapeCircle&>(lhs_shape);
    auto& rhs_circle = static_cast<const CollisionShapeCircle&>(rhs_shape);

    auto lhs_center = transform_by(lhs_circle.center(), lhs_transform);
    auto lhs_radius = lhs_circle.radius() * max_side(lhs_transform.scale);
    auto rhs_center = transform_by(rhs_circle.center(), rhs_transform);
    auto rhs_radius = rhs_circle.radius() * max_side(rhs_transform.scale);

    auto radius = lhs_radius + rhs_radius;
    auto distanced_squared = calc_distanced_squared(lhs_center, rhs_center);
    if (distanced_squared > radius*radius)
        return CollisionShape::CollisionResult {};

    auto normal = glm::normalize(lhs_center - rhs_center);
    auto penetration = abs(sqrt(distanced_squared) - radius);
    return CollisionShape::CollisionResult
    {
        .is_colliding = true,
        .penetration_distance = penetration,
        .normal = normal,
        .intersection_points = { lhs_center + normal * lhs_radius },
        .intersection_point_count = 1,
    };
}

static CollisionShape::CollisionResult collide_aabb_circle(
    const CollisionShape& lhs_shape, const Transform::Computed2D& lhs_transform,
    const CollisionShape& rhs_shape, const Transform::Computed2D& rhs_transform)
{
    return CollisionShape::CollisionResult {};

    auto& lhs_aabb = static_cast<const CollisionShapeAABB&>(lhs_shape);
    auto& rhs_circle = static_cast<const CollisionShapeCircle&>(rhs_shape);

    CollisionShapeCircle bounding_circle(lhs_aabb.center(), lhs_aabb.half_widths().length());
    auto bouding_result = collide_circle_circle(bounding_circle, lhs_transform, rhs_circle, rhs_transform);
    if (!bouding_result.is_colliding)
        return bouding_result;

    auto lhs_center = transform_by(lhs_aabb.center(), lhs_transform);
    auto lhs_half_widths = lhs_aabb.half_widths() * lhs_transform.scale;
    auto rhs_center = transform_by(rhs_circle.center(), rhs_transform);
    auto rhs_radius = rhs_circle.radius() * max_side(rhs_transform.scale);
    auto circle_intersect_point = bouding_result.normal * rhs_radius + rhs_center;

    auto penetration_x = abs(circle_intersect_point.x - lhs_center.x) - lhs_half_widths.x;
    if (penetration_x > 0)
        return CollisionShape::CollisionResult {};

    auto penetration_y = abs(circle_intersect_point.y - lhs_center.y) - lhs_half_widths.y;
    if (penetration_y > 0)
        return CollisionShape::CollisionResult {};

    auto normal = bouding_result.normal;
    auto penetration = abs(penetration_x * normal.x + penetration_y * normal.y);
    return CollisionShape::CollisionResult
    {
        .is_colliding = true,
        .penetration_distance = penetration,
        .normal = normal,
        .intersection_points = { circle_intersect_point },
        .intersection_point_count = 1,
    };
}

static CollisionShape::CollisionResult collide_aabb_obb(
    const CollisionShape& lhs_shape, const Transform::Computed2D& lhs_transform,
    const CollisionShape& rhs_shape, const Transform::Computed2D& rhs_transform)
{
    auto& lhs_aabb = static_cast<const CollisionShapeAABB&>(lhs_shape);
    auto& rhs_obb = static_cast<const CollisionShapeOBB&>(rhs_shape);

    auto lhs_points = aabb_points(lhs_aabb, lhs_transform);
    auto rhs_points = obb_points(rhs_obb, rhs_transform);
    return collide_convex_polygons(lhs_points, rhs_points);
}

static CollisionShape::CollisionResult collide_obb_circle(
    const CollisionShape& lhs_shape, const Transform::Computed2D& lhs_transform,
    const CollisionShape& rhs_shape, const Transform::Computed2D& rhs_transform)
{
    auto& lhs_obb = static_cast<const CollisionShapeOBB&>(lhs_shape);
    auto& rhs_circle = static_cast<const CollisionShapeCircle&>(rhs_shape);

    mat4 to_lhs_space(1);
    to_lhs_space = glm::rotate(to_lhs_space, -lhs_obb.rotation(), vec3(0, 0, 1));
    to_lhs_space = glm::translate(to_lhs_space, vec3(-lhs_obb.center(), 0));
    to_lhs_space = glm::scale(to_lhs_space, vec3(1.0f / lhs_transform.scale, 0));
    to_lhs_space = glm::rotate(to_lhs_space, -lhs_transform.rotation, vec3(0, 0, 1));
    to_lhs_space = glm::translate(to_lhs_space, vec3(-lhs_transform.position, 0));

    auto lhs_half_widths = lhs_obb.half_widths();
    auto rhs_center = transform_by(rhs_circle.center(), rhs_transform);
    auto rhs_radius = rhs_circle.radius() * max_side(rhs_transform.scale);
    auto rhs_center_in_lhs_space = vec2(to_lhs_space * vec4(rhs_center, 0, 1));

    auto distance_to_center_sqaured = glm::length2(rhs_center_in_lhs_space);
    auto lhs_radius = max_side(lhs_half_widths);
    if (distance_to_center_sqaured > (lhs_radius + rhs_radius) * (lhs_radius + rhs_radius))
        return CollisionShape::CollisionResult {};

    auto direction_to_center = -glm::normalize(rhs_center_in_lhs_space);
    auto circle_intersect_point = rhs_center_in_lhs_space + direction_to_center * rhs_radius;

    auto penetration_x = std::abs(circle_intersect_point.x) - lhs_half_widths.x;
    auto penetration_y = std::abs(circle_intersect_point.y) - lhs_half_widths.y;
    if (penetration_x > 0 || penetration_y > 0)
        return CollisionShape::CollisionResult {};

    vec2 normal_lhs_space(0, 0);
    if (penetration_x < penetration_y)
        normal_lhs_space = vec2(0, rhs_center_in_lhs_space.y > 0 ? -1 : 1);
    if (penetration_x > penetration_y)
        normal_lhs_space = vec2(rhs_center_in_lhs_space.x > 0 ? -1 : 1, 0);

    auto normal = vec2(
        glm::rotate(lhs_obb.rotation() + lhs_transform.rotation, vec3(0, 0, 1)) *
        vec4(normal_lhs_space, 0, 1));

    auto intersection_point = vec2(
        lhs_transform.transform *
        vec4(normal * lhs_obb.half_widths() + lhs_obb.center(), 0, 1));

    auto penetration = penetration_x < penetration_y
        ? -penetration_y * lhs_transform.scale.y
        : -penetration_x * lhs_transform.scale.x;
    return CollisionShape::CollisionResult
    {
        .is_colliding = true,
        .penetration_distance = penetration,
        .normal = normal,
        .intersection_points = { intersection_point },
        .intersection_point_count = 1,
    };
}

static CollisionShape::CollisionResult collide_obb_obb(
    const CollisionShape& lhs_shape, const Transform::Computed2D& lhs_transform,
    const CollisionShape& rhs_shape, const Transform::Computed2D& rhs_transform)
{
    auto& lhs_obb = static_cast<const CollisionShapeOBB&>(lhs_shape);
    auto& rhs_obb = static_cast<const CollisionShapeOBB&>(rhs_shape);

    auto lhs_points = obb_points(lhs_obb, lhs_transform);
    auto rhs_points = obb_points(rhs_obb, rhs_transform);
    return collide_convex_polygons(lhs_points, rhs_points);
}

CollisionShape::CollisionResult check_collisions_for_colliders(
    const CollisionShape &lhs_shape, const Transform::Computed2D &lhs_transform,
    const CollisionShape &rhs_shape, const Transform::Computed2D &rhs_transform)
{
    auto flipped = [](CollisionShape::CollisionResult &&result)
    {
        result.normal = -result.normal;
        return result;
    };
    
    switch (lhs_shape.type())
    {
        case CollisionShape::Type::AABB:
            switch (rhs_shape.type())
            {
                case CollisionShape::Type::AABB:
                    return collide_aabb_aabb(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
                case CollisionShape::Type::OBB:
                    return collide_aabb_obb(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
                case CollisionShape::Type::Circle:
                    return collide_aabb_circle(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
            }
        case CollisionShape::Type::OBB:
            switch (rhs_shape.type())
            {
                case CollisionShape::Type::AABB:
                    return flipped(collide_aabb_obb(rhs_shape, rhs_transform, lhs_shape, lhs_transform));
                case CollisionShape::Type::OBB:
                    return collide_obb_obb(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
                case CollisionShape::Type::Circle:
                    return collide_obb_circle(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
            }
        case CollisionShape::Type::Circle:
            switch (rhs_shape.type())
            {
                case CollisionShape::Type::AABB:
                    return flipped(collide_aabb_circle(rhs_shape, rhs_transform, lhs_shape, lhs_transform));
                case CollisionShape::Type::OBB:
                    return flipped(collide_obb_circle(rhs_shape, rhs_transform, lhs_shape, lhs_transform));
                case CollisionShape::Type::Circle:
                    return collide_circle_circle(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
            }
    }

    assert (false);
}

CollisionShape::CollisionResult CollisionShape::check_collisions(
    const GameObject &lhs, const Collider &lhs_collider, 
    const GameObject &rhs, const Collider &rhs_collider)
{
    auto lhs_transform = lhs.first<Transform>()->computed_transform_2d();
    auto rhs_transform = rhs.first<Transform>()->computed_transform_2d();

    auto result = check_collisions_for_colliders(
        lhs_collider.shape(), lhs_transform, rhs_collider.shape(), rhs_transform);
    return result;
}

static vec2 rotate(vec2 vec, float angle)
{
    vec4 result = glm::rotate(mat4(1), angle, vec3(0, 0, 1)) * vec4(vec, 0, 1);
    return vec2(result.x, result.y);
}


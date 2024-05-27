/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "collision_shape_2d.hpp"
#include "collision_shape_utils_2d.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/physics/collider_2d.hpp"
#include "gameobject/transform.hpp"
#include <cstdlib>
#include <glm/gtx/transform.hpp>
using namespace Engine;
using namespace Object;

CollisionShape2D::~CollisionShape2D()
{
}

static CollisionShape2D::CollisionResult collide_aabb_aabb(
    CollisionShape2D const& lhs_shape, Transform::Computed2D const& lhs_transform,
    CollisionShape2D const& rhs_shape, Transform::Computed2D const& rhs_transform)
{
    auto const& lhs_aabb = static_cast<CollisionShapeAABB2D const&>(lhs_shape);
    auto const& rhs_aabb = static_cast<CollisionShapeAABB2D const&>(rhs_shape);

    auto lhs_center = transform_by(lhs_aabb.center(), lhs_transform);
    auto lhs_half_widths = lhs_aabb.half_widths() * lhs_transform.scale;
    auto rhs_center = transform_by(rhs_aabb.center(), rhs_transform);
    auto rhs_half_widths = rhs_aabb.half_widths() * rhs_transform.scale;

    auto penetration_x = std::abs(lhs_center.x - rhs_center.x) - (lhs_half_widths.x + rhs_half_widths.x);
    if (penetration_x > 0) {
        return CollisionShape2D::CollisionResult {};
    }

    auto penetration_y = std::abs(lhs_center.y - rhs_center.y) - (lhs_half_widths.y + rhs_half_widths.y);
    if (penetration_y > 0) {
        return CollisionShape2D::CollisionResult {};
    }

    glm::vec2 normal(0, 0);
    if (penetration_x < penetration_y) {
        normal = glm::vec2(0, lhs_center.y < rhs_center.y ? -1 : 1);
    }
    if (penetration_x > penetration_y) {
        normal = glm::vec2(lhs_center.x < rhs_center.x ? -1 : 1, 0);
    }

    auto penetration = std::abs(penetration_x * normal.x + penetration_y * normal.y);
    return CollisionShape2D::CollisionResult {
        .is_colliding = true,
        .penetration_distance = penetration,
        .normal = normal,
        .intersection_points = { rhs_center + normal * rhs_half_widths },
        .intersection_point_count = 1,
    };
}

static CollisionShape2D::CollisionResult collide_circle_circle(
    CollisionShape2D const& lhs_shape, Transform::Computed2D const& lhs_transform,
    CollisionShape2D const& rhs_shape, Transform::Computed2D const& rhs_transform)
{
    auto const& lhs_circle = static_cast<CollisionShapeCircle2D const&>(lhs_shape);
    auto const& rhs_circle = static_cast<CollisionShapeCircle2D const&>(rhs_shape);

    auto lhs_center = transform_by(lhs_circle.center(), lhs_transform);
    auto lhs_radius = lhs_circle.radius() * max_side(lhs_transform.scale);
    auto rhs_center = transform_by(rhs_circle.center(), rhs_transform);
    auto rhs_radius = rhs_circle.radius() * max_side(rhs_transform.scale);

    auto radius = lhs_radius + rhs_radius;
    auto distanced_squared = calc_distanced_squared(lhs_center, rhs_center);
    if (distanced_squared > radius * radius) {
        return CollisionShape2D::CollisionResult {};
    }

    auto normal = glm::normalize(lhs_center - rhs_center);
    auto penetration = std::abs(std::sqrt(distanced_squared) - radius);
    return CollisionShape2D::CollisionResult {
        .is_colliding = true,
        .penetration_distance = penetration,
        .normal = normal,
        .intersection_points = { lhs_center + normal * lhs_radius },
        .intersection_point_count = 1,
    };
}

static CollisionShape2D::CollisionResult collide_aabb_circle(
    CollisionShape2D const& lhs_shape, Transform::Computed2D const& lhs_transform,
    CollisionShape2D const& rhs_shape, Transform::Computed2D const& rhs_transform)
{
    return CollisionShape2D::CollisionResult {};

    auto const& lhs_aabb = static_cast<CollisionShapeAABB2D const&>(lhs_shape);
    auto const& rhs_circle = static_cast<CollisionShapeCircle2D const&>(rhs_shape);

    CollisionShapeCircle2D const bounding_circle(lhs_aabb.center(), lhs_aabb.half_widths().length());
    auto bouding_result = collide_circle_circle(bounding_circle, lhs_transform, rhs_circle, rhs_transform);
    if (!bouding_result.is_colliding) {
        return bouding_result;
    }

    auto lhs_center = transform_by(lhs_aabb.center(), lhs_transform);
    auto lhs_half_widths = lhs_aabb.half_widths() * lhs_transform.scale;
    auto rhs_center = transform_by(rhs_circle.center(), rhs_transform);
    auto rhs_radius = rhs_circle.radius() * max_side(rhs_transform.scale);
    auto circle_intersect_point = bouding_result.normal * rhs_radius + rhs_center;

    auto penetration_x = std::abs(circle_intersect_point.x - lhs_center.x) - lhs_half_widths.x;
    if (penetration_x > 0) {
        return CollisionShape2D::CollisionResult {};
    }

    auto penetration_y = std::abs(circle_intersect_point.y - lhs_center.y) - lhs_half_widths.y;
    if (penetration_y > 0) {
        return CollisionShape2D::CollisionResult {};
    }

    auto normal = bouding_result.normal;
    auto penetration = std::abs(penetration_x * normal.x + penetration_y * normal.y);
    return CollisionShape2D::CollisionResult {
        .is_colliding = true,
        .penetration_distance = penetration,
        .normal = normal,
        .intersection_points = { circle_intersect_point },
        .intersection_point_count = 1,
    };
}

static CollisionShape2D::CollisionResult collide_aabb_obb(
    CollisionShape2D const& lhs_shape, Transform::Computed2D const& lhs_transform,
    CollisionShape2D const& rhs_shape, Transform::Computed2D const& rhs_transform)
{
    auto const& lhs_aabb = static_cast<CollisionShapeAABB2D const&>(lhs_shape);
    auto const& rhs_obb = static_cast<CollisionShapeOBB2D const&>(rhs_shape);

    auto lhs_points = aabb_points(lhs_aabb, lhs_transform);
    auto rhs_points = obb_points(rhs_obb, rhs_transform);
    return collide_convex_polygons(lhs_points, rhs_points);
}

static CollisionShape2D::CollisionResult collide_obb_circle(
    CollisionShape2D const& lhs_shape, Transform::Computed2D const& lhs_transform,
    CollisionShape2D const& rhs_shape, Transform::Computed2D const& rhs_transform)
{
    auto const& lhs_obb = static_cast<CollisionShapeOBB2D const&>(lhs_shape);
    auto const& rhs_circle = static_cast<CollisionShapeCircle2D const&>(rhs_shape);

    glm::mat4 to_lhs_space(1);
    to_lhs_space = glm::rotate(to_lhs_space, -lhs_obb.rotation(), glm::vec3(0, 0, 1));
    to_lhs_space = glm::translate(to_lhs_space, glm::vec3(-lhs_obb.center(), 0));
    to_lhs_space = glm::scale(to_lhs_space, glm::vec3(1.0f / lhs_transform.scale, 0));
    to_lhs_space = glm::rotate(to_lhs_space, -lhs_transform.rotation, glm::vec3(0, 0, 1));
    to_lhs_space = glm::translate(to_lhs_space, glm::vec3(-lhs_transform.position, 0));

    auto lhs_half_widths = lhs_obb.half_widths();
    auto rhs_center = transform_by(rhs_circle.center(), rhs_transform);
    auto rhs_radius = rhs_circle.radius() * max_side(rhs_transform.scale);
    auto rhs_center_in_lhs_space = glm::vec2(to_lhs_space * glm::vec4(rhs_center, 0, 1));

    auto distance_to_center_sqaured = glm::length(rhs_center_in_lhs_space);
    auto lhs_radius = max_side(lhs_half_widths);
    if (distance_to_center_sqaured > (lhs_radius + rhs_radius) * (lhs_radius + rhs_radius)) {
        return CollisionShape2D::CollisionResult {};
    }

    auto direction_to_center = -glm::normalize(rhs_center_in_lhs_space);
    auto circle_intersect_point = rhs_center_in_lhs_space + direction_to_center * rhs_radius;

    auto penetration_x = std::abs(circle_intersect_point.x) - lhs_half_widths.x;
    auto penetration_y = std::abs(circle_intersect_point.y) - lhs_half_widths.y;
    if (penetration_x > 0 || penetration_y > 0) {
        return CollisionShape2D::CollisionResult {};
    }

    glm::vec2 normal_lhs_space(0, 0);
    if (penetration_x < penetration_y) {
        normal_lhs_space = glm::vec2(0, rhs_center_in_lhs_space.y > 0 ? -1 : 1);
    }
    if (penetration_x > penetration_y) {
        normal_lhs_space = glm::vec2(rhs_center_in_lhs_space.x > 0 ? -1 : 1, 0);
    }

    auto normal = glm::vec2(
        glm::rotate(lhs_obb.rotation() + lhs_transform.rotation, glm::vec3(0, 0, 1)) * glm::vec4(normal_lhs_space, 0, 1));

    auto intersection_point = glm::vec2(
        lhs_transform.transform * glm::vec4(normal * lhs_obb.half_widths() + lhs_obb.center(), 0, 1));

    auto penetration = penetration_x < penetration_y
        ? -penetration_y * lhs_transform.scale.y
        : -penetration_x * lhs_transform.scale.x;
    return CollisionShape2D::CollisionResult {
        .is_colliding = true,
        .penetration_distance = penetration,
        .normal = normal,
        .intersection_points = { intersection_point },
        .intersection_point_count = 1,
    };
}

static CollisionShape2D::CollisionResult collide_obb_obb(
    CollisionShape2D const& lhs_shape, Transform::Computed2D const& lhs_transform,
    CollisionShape2D const& rhs_shape, Transform::Computed2D const& rhs_transform)
{
    auto const& lhs_obb = static_cast<CollisionShapeOBB2D const&>(lhs_shape);
    auto const& rhs_obb = static_cast<CollisionShapeOBB2D const&>(rhs_shape);

    auto lhs_points = obb_points(lhs_obb, lhs_transform);
    auto rhs_points = obb_points(rhs_obb, rhs_transform);
    return collide_convex_polygons(lhs_points, rhs_points);
}

CollisionShape2D::CollisionResult check_collisions_for_colliders(
    CollisionShape2D const& lhs_shape, Transform::Computed2D const& lhs_transform,
    CollisionShape2D const& rhs_shape, Transform::Computed2D const& rhs_transform)
{
    auto flipped = [](CollisionShape2D::CollisionResult&& result) {
        result.normal = -result.normal;
        return result;
    };

    switch (lhs_shape.type()) {
    case CollisionShape2D::Type::AABB:
        switch (rhs_shape.type()) {
        case CollisionShape2D::Type::AABB:
            return collide_aabb_aabb(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
        case CollisionShape2D::Type::OBB:
            return collide_aabb_obb(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
        case CollisionShape2D::Type::Circle:
            return collide_aabb_circle(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
        }
    case CollisionShape2D::Type::OBB:
        switch (rhs_shape.type()) {
        case CollisionShape2D::Type::AABB:
            return flipped(collide_aabb_obb(rhs_shape, rhs_transform, lhs_shape, lhs_transform));
        case CollisionShape2D::Type::OBB:
            return collide_obb_obb(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
        case CollisionShape2D::Type::Circle:
            return collide_obb_circle(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
        }
    case CollisionShape2D::Type::Circle:
        switch (rhs_shape.type()) {
        case CollisionShape2D::Type::AABB:
            return flipped(collide_aabb_circle(rhs_shape, rhs_transform, lhs_shape, lhs_transform));
        case CollisionShape2D::Type::OBB:
            return flipped(collide_obb_circle(rhs_shape, rhs_transform, lhs_shape, lhs_transform));
        case CollisionShape2D::Type::Circle:
            return collide_circle_circle(lhs_shape, lhs_transform, rhs_shape, rhs_transform);
        }
    }

    assert(false);
}

CollisionShape2D::CollisionResult CollisionShape2D::check_collisions(
    GameObject const& lhs, Collider2D const& lhs_collider,
    GameObject const& rhs, Collider2D const& rhs_collider)
{
    auto lhs_transform = lhs.first<Transform>()->computed_transform_2d();
    auto rhs_transform = rhs.first<Transform>()->computed_transform_2d();

    auto result = check_collisions_for_colliders(
        lhs_collider.shape(), lhs_transform, rhs_collider.shape(), rhs_transform);
    return result;
}

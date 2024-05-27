/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "collision_shape_utils_2d.hpp"
#include "gameobject/gameobject.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <optional>
using namespace Engine;
using namespace Object;

float Engine::calc_distanced_squared(glm::vec2 point_a, glm::vec2 point_b)
{
    auto x = point_a.x - point_b.x;
    auto y = point_a.y - point_b.y;
    return x * x + y * y;
}

float Engine::max_side(glm::vec2 vec)
{
    return std::max(vec.x, vec.y);
}

glm::vec2 Engine::vec_from_angle(float angle)
{
    return glm::vec2(std::cos(angle), std::sin(angle));
}

glm::vec2 Engine::vec_3to2(glm::vec3 vec)
{
    return glm::vec2(vec.x, vec.z);
}

glm::vec3 Engine::vec_2to3(glm::vec2 vec)
{
    return glm::vec3(vec.x, 0, vec.y);
}

glm::vec2 Engine::transform_by(glm::vec2 position, Transform::Computed2D const& transform)
{
    auto result = transform.transform * glm::vec4(position, 0, 1);
    return glm::vec2(result.x, result.y);
}

std::vector<glm::vec4> Engine::aabb_points(
    CollisionShapeAABB2D const& aabb, Transform::Computed2D const& transform)
{
    auto center = transform_by(aabb.center(), transform);
    auto half_widths = aabb.half_widths() * transform.scale;
    return std::vector<glm::vec4> {
        glm::vec4(center + glm::vec2(-half_widths.x, half_widths.y), 0, 1),
        glm::vec4(center + glm::vec2(half_widths.x, half_widths.y), 0, 1),
        glm::vec4(center + glm::vec2(half_widths.x, -half_widths.y), 0, 1),
        glm::vec4(center + glm::vec2(-half_widths.x, -half_widths.y), 0, 1),
    };
}

std::vector<glm::vec4> Engine::obb_points(
    CollisionShapeOBB2D const& obb, Transform::Computed2D const& transform)
{
    auto center = obb.center();
    auto half_widths = obb.half_widths();

    auto to_world_space_transform = glm::translate(glm::vec3(center, 0)) * glm::rotate(obb.rotation(), glm::vec3(0, 0, 1)) * transform.transform;

    return std::vector<glm::vec4> {
        to_world_space_transform * glm::vec4(-half_widths.x, half_widths.y, 0, 1),
        to_world_space_transform * glm::vec4(half_widths.x, half_widths.y, 0, 1),
        to_world_space_transform * glm::vec4(half_widths.x, -half_widths.y, 0, 1),
        to_world_space_transform * glm::vec4(-half_widths.x, -half_widths.y, 0, 1),
    };
}

std::vector<float> Engine::find_axes_from_points(std::vector<glm::vec4> const& points)
{
    std::vector<float> axes;
    axes.reserve(points.size());

    for (int i = 0; i < points.size(); i++) {
        auto point_a = points[i];
        auto point_b = points[(i + 1) % points.size()];
        auto normal = glm::normalize(point_b - point_a);
        axes.push_back(glm::orientedAngle(glm::vec2(-normal.x, normal.y), glm::vec2(0, 1)));
    }
    return axes;
}

std::tuple<float, glm::vec2, float, glm::vec2> Engine::find_max_min_points_along_axis(std::vector<glm::vec4> const& points, glm::vec2 axis)
{
    auto min_point_along_axis = std::numeric_limits<float>::infinity();
    auto max_point_along_axis = -std::numeric_limits<float>::infinity();
    glm::vec2 min_point;
    glm::vec2 max_point;
    for (auto point : points) {
        auto position_along_axis = glm::dot(glm::vec2(point.x, point.y), axis);
        if (position_along_axis < min_point_along_axis) {
            min_point_along_axis = position_along_axis;
            min_point = point;
        }

        if (position_along_axis > max_point_along_axis) {
            max_point_along_axis = position_along_axis;
            max_point = point;
        }
    }

    return std::make_tuple(min_point_along_axis, min_point, max_point_along_axis, max_point);
}

static CollisionShape2D::CollisionResult collide_convex_polygons_with_axes(
    std::vector<glm::vec4> const& lhs_points, std::vector<glm::vec4> const& rhs_points, std::vector<float> axes)
{
    auto is_axis_overlapping = [&](glm::vec2 axis) {
        auto [lhs_min, lhs_min_point, lhs_max, lhs_max_point] = find_max_min_points_along_axis(lhs_points, axis);
        auto [rhs_min, rhs_min_point, rhs_max, rhs_max_point] = find_max_min_points_along_axis(rhs_points, axis);

        float const lhs_penetration = lhs_max - rhs_min;
        if (lhs_penetration < 0) {
            return CollisionShape2D::CollisionResult {};
        }

        float const rhs_penetration = rhs_max - lhs_min;
        if (rhs_penetration < 0) {
            return CollisionShape2D::CollisionResult {};
        }

        auto penetration = std::min(lhs_penetration, rhs_penetration);
        auto normal = lhs_penetration > rhs_penetration ? axis : -axis;
        return CollisionShape2D::CollisionResult {
            .is_colliding = true,
            .penetration_distance = penetration,
            .normal = normal,
        };
    };

    CollisionShape2D::CollisionResult min_result {
        .penetration_distance = std::numeric_limits<float>::infinity(),
    };

    int intersection_point_count;
    float const penetration = std::numeric_limits<float>::infinity();
    for (auto axis : axes) {
        auto result = is_axis_overlapping(vec_from_angle(axis));
        if (!result.is_colliding) {
            return CollisionShape2D::CollisionResult {};
        }

        if (result.penetration_distance < min_result.penetration_distance) {
            min_result = result;
        }
    }

    return min_result;
}

struct Face {
    glm::vec2 a;
    glm::vec2 b;
    glm::vec2 normal;
};

static glm::vec2 adjacent(glm::vec2 const& vec)
{
    return glm::vec2(-vec.y, vec.x);
}

template<typename Callback>
static void for_each_face(std::vector<glm::vec4> const& points, Callback callback)
{
    for (int i = 0; i < points.size(); i++) {
        auto const& a = points[i];
        auto const& b = points[(i + 1) % points.size()];
        auto normal = adjacent(glm::normalize(glm::vec2(b.x, b.y) - glm::vec2(a.x, a.y)));
        auto decision = callback(Face {
            .a = glm::vec2(a.x, a.y),
            .b = glm::vec2(b.x, b.y),
            .normal = normal,
        });

        if (decision == IteratorDecision::Break) {
            break;
        }
    }
}

static float find_support_point(std::vector<glm::vec4> const& points, glm::vec2 const& axis)
{
    std::optional<float> min_point = std::nullopt;
    for (auto const& point : points) {
        auto point_along_axis = glm::dot(axis, glm::vec2(point.x, point.y));
        if (!min_point || point_along_axis < *min_point) {
            min_point = point_along_axis;
        }
    }

    assert(min_point);
    return *min_point;
}

static std::pair<std::optional<Face>, float> find_intersecting_face(
    std::vector<glm::vec4> const& lhs_points, std::vector<glm::vec4> const& rhs_points)
{
    float min_penetration = std::numeric_limits<float>::infinity();
    std::optional<Face> min_face;

    for_each_face(lhs_points, [&](Face face) {
        auto support_point = find_support_point(rhs_points, face.normal);
        auto penetration = glm::dot(face.normal, face.a) - support_point;
        if (penetration < 0) {
            min_face = std::nullopt;
            return IteratorDecision::Break;
        }

        if (penetration <= min_penetration) {
            min_penetration = penetration;
            min_face = face;
        }
        return IteratorDecision::Continue;
    });

    return std::make_pair(min_face, min_penetration);
}

static bool clip(Face& face, glm::vec2 const& normal, float line)
{
    glm::vec2 out[2];
    int sp = 0;

    auto distance_a = glm::dot(normal, face.a) - line;
    auto distance_b = glm::dot(normal, face.b) - line;

    // If negative (behind plane) clip
    if (distance_a <= 0.0f) {
        out[sp++] = face.a;
    }
    if (distance_b <= 0.0f) {
        out[sp++] = face.b;
    }

    // If the points are on different sides of the plane
    if (distance_a * distance_b < 0.0f) { // less than to ignore -0.0f
        // Push intersection point
        auto alpha = distance_a / (distance_a - distance_b);
        out[sp++] = face.a + alpha * (face.b - face.a);
    }

    if (sp < 2) {
        return false;
    }

    // Assign our new converted values
    face.a = out[0];
    face.b = out[1];
    return true;
}

inline float find_penetration(float separation_a, float separation_b)
{
    if (separation_a >= 0 && separation_b >= 0) {
        return 0;
    }

    if (separation_a >= 0) {
        return -separation_b;
    }
    if (separation_b >= 0) {
        return -separation_a;
    }

    return std::min(-separation_a, -separation_b);
}

CollisionShape2D::CollisionResult Engine::collide_convex_polygons(
    std::vector<glm::vec4> const& lhs_points, std::vector<glm::vec4> const& rhs_points)
{
    auto [lhs_face, lhs_penetration] = find_intersecting_face(lhs_points, rhs_points);
    if (!lhs_face) {
        return CollisionShape2D::CollisionResult {};
    }

    auto const [rhs_face, rhs_penetration] = find_intersecting_face(rhs_points, lhs_points);
    if (!rhs_face) {
        return CollisionShape2D::CollisionResult {};
    }

    auto const& reference_face = lhs_penetration > rhs_penetration ? *lhs_face : *rhs_face;
    auto incident_face = lhs_penetration > rhs_penetration ? *rhs_face : *lhs_face;
    auto side_plane_normal = -adjacent(reference_face.normal);

    auto negative_side = -glm::dot(side_plane_normal, reference_face.a);
    if (!clip(incident_face, -side_plane_normal, negative_side)) {
        return CollisionShape2D::CollisionResult {};
    }

    auto positive_side = glm::dot(side_plane_normal, reference_face.b);
    if (!clip(incident_face, side_plane_normal, positive_side)) {
        return CollisionShape2D::CollisionResult {};
    }

    auto ref_line = glm::dot(reference_face.normal, reference_face.a);

    auto separation_a = glm::dot(reference_face.normal, incident_face.a) - ref_line;
    auto separation_b = glm::dot(reference_face.normal, incident_face.b) - ref_line;

    auto normal = lhs_penetration > rhs_penetration ? incident_face.normal : -incident_face.normal;
    auto penetration = find_penetration(separation_a, separation_b);
    auto result = CollisionShape2D::CollisionResult {
        .is_colliding = true,
        .penetration_distance = penetration,
        .normal = normal,
    };

    if (separation_a < 0) {
        result.intersection_points[result.intersection_point_count++] = incident_face.a;
    }
    if (separation_b < 0) {
        result.intersection_points[result.intersection_point_count++] = incident_face.b;
    }

    return result;
}

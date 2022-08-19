/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once
#include "engine/physics/collision_shape_2d.hpp"
#include "gameobject/transform.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Engine
{

    float calc_distanced_squared(glm::vec2 point_a, glm::vec2 point_b);
    float max_side(glm::vec2 vec);
    glm::vec2 vec_from_angle(float angle);

    glm::vec2 vec_3to2(glm::vec3 vec);
    glm::vec3 vec_2to3(glm::vec2 vec);
    glm::vec2 transform_by(glm::vec2 position, const Object::Transform::Computed2D &transform);

    std::vector<glm::vec4> aabb_points(
        const CollisionShapeAABB2D &aabb, const Object::Transform::Computed2D &transform);
    std::vector<glm::vec4> obb_points(
        const CollisionShapeOBB2D &obb, const Object::Transform::Computed2D &transform);

    std::vector<float> find_axes_from_points(const std::vector<glm::vec4> &points);
    std::tuple<float, glm::vec2, float, glm::vec2> find_max_min_points_along_axis(
        const std::vector<glm::vec4>& points, glm::vec2 axis);

    CollisionShape2D::CollisionResult collide_convex_polygons(
        const std::vector<glm::vec4>& lhs_points, const std::vector<glm::vec4>& rhs_points);

}


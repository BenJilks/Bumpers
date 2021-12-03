#pragma one
#include "engine/physics/collision_shape.hpp"
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

    std::vector<glm::vec4> aabb_points(
        const CollisionShapeAABB &aabb, const Object::Transform::Computed &transform);
    std::vector<glm::vec4> obb_points(
        const CollisionShapeOBB &obb, const Object::Transform::Computed &transform);
    std::vector<glm::vec4> convex_polygon_points(
        const CollisionShapeConvexPolygon &convex_polygon, const Object::Transform::Computed &transform);

    std::vector<float> find_axes_from_points(const std::vector<glm::vec4> &points);
    std::tuple<float, glm::vec2, float, glm::vec2> find_max_min_points_along_axis(
        const std::vector<glm::vec4>& points, glm::vec2 axis);

    CollisionShape::CollisionResult collide_convex_polygons(
        const std::vector<glm::vec4>& lhs_points, const std::vector<glm::vec4>& rhs_points);

}


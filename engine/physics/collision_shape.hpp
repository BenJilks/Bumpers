#pragma once

#include "gameobject/forward.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include <optional>

#include <iostream>

namespace Engine
{

    class CollisionShape
    {
    public:
        virtual ~CollisionShape();
        
        struct CollisionResult
        {
            bool is_colliding { false };
            float penetration_distance { 0 };
            glm::vec2 normal { 0, 0 };

            glm::vec2 intersection_points[2];
            int intersection_point_count { 0 };
        };

        static CollisionResult check_collisions(
            const Object::GameObject& lhs, const Object::Collider& lhs_collider, 
            const Object::GameObject& rhs, const Object::Collider& rhs_collider);

        enum class Type
        {
            AABB,
            OBB,
            Circle,
            ConvexPolygon,
        };

        virtual Type type() const = 0;

    };

    class CollisionShapeAABB : public CollisionShape
    {
    public:
        CollisionShapeAABB(glm::vec2 center, glm::vec2 half_widths)
            : m_center(center)
            , m_half_widths(half_widths)
        {
        }

        virtual Type type() const override { return Type::AABB; }
        inline const glm::vec2 center() const { return m_center; }
        inline const glm::vec2 half_widths() const { return m_half_widths; }

    private:
        glm::vec2 m_center;
        glm::vec2 m_half_widths;

    };

    class CollisionShapeOBB : public CollisionShapeAABB
    {
    public:
        CollisionShapeOBB(glm::vec2 center, glm::vec2 half_widths)
            : CollisionShapeAABB(center, half_widths)
        {
        }

        virtual Type type() const final { return Type::OBB; }

    };

    class CollisionShapeCircle : public CollisionShape
    {
    public:
        CollisionShapeCircle(glm::vec2 center, float radius)
            : m_center(center)
            , m_radius(radius)
        {
        }

        virtual Type type() const final { return Type::Circle; }
        inline const glm::vec2 &center() const { return m_center; }
        inline float radius() const { return m_radius; }

    private:
        glm::vec2 m_center;
        float m_radius;

    };

    class CollisionShapeConvexPolygon : public CollisionShape
    {
    public:
        CollisionShapeConvexPolygon(const CollisionShapeConvexPolygon&) = delete;

        CollisionShapeConvexPolygon(const std::vector<glm::vec2> verticies)
            : m_verticies(verticies)
        {
        }
        
        template<typename... Args>
        CollisionShapeConvexPolygon(Args... verticies)
            : CollisionShapeConvexPolygon(std::vector<glm::vec2> { verticies... })
        {
        }

        virtual Type type() const final { return Type::ConvexPolygon; }
        inline const std::vector<glm::vec2> &verticies() const { return m_verticies; }
        std::pair<glm::vec2, glm::vec2> bounding_box(glm::vec2 scale, float rotation);
        
    private:
        std::vector<glm::vec2> m_verticies;

        float m_cached_rotation { 0 };
        glm::vec2 m_cached_scale { 1, 1 };
        std::optional<std::pair<glm::vec2, glm::vec2>> m_bounding_box_cache { std::nullopt };
        
    };

}


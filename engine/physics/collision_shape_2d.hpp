#pragma once

#include "gameobject/forward.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include <optional>

#include <iostream>

namespace Engine
{

    class CollisionShape2D
    {
    public:
        virtual ~CollisionShape2D();
        
        struct CollisionResult
        {
            bool is_colliding { false };
            float penetration_distance { 0 };
            glm::vec2 normal { 0, 0 };

            glm::vec2 intersection_points[2];
            int intersection_point_count { 0 };
        };

        static CollisionResult check_collisions(
            const Object::GameObject& lhs, const Object::Collider2D& lhs_collider, 
            const Object::GameObject& rhs, const Object::Collider2D& rhs_collider);

        enum class Type
        {
            AABB,
            OBB,
            Circle,
        };

        virtual Type type() const = 0;

    };

    class CollisionShapeAABB2D : public CollisionShape2D
    {
    public:
        CollisionShapeAABB2D(glm::vec2 center, glm::vec2 half_widths)
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

    class CollisionShapeOBB2D : public CollisionShapeAABB2D
    {
    public:
        CollisionShapeOBB2D(glm::vec2 center, glm::vec2 half_widths, float rotation = 0)
            : CollisionShapeAABB2D(center, half_widths)
            , m_rotation(rotation)
        {
        }

        virtual Type type() const final { return Type::OBB; }
        inline const float rotation() const { return m_rotation; }

    private:
        float m_rotation;

    };

    class CollisionShapeCircle2D : public CollisionShape2D
    {
    public:
        CollisionShapeCircle2D(glm::vec2 center, float radius)
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

}


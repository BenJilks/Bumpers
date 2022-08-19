/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "collision_resolver_2d.hpp"
#include "collision_shape_2d.hpp"
#include "collision_shape_utils_2d.hpp"
#include "broad_phase_collision_2d.hpp"
#include "gameobject/world.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/physics/physics_body_2d.hpp"
#include "gameobject/physics/collider_2d.hpp"
#include <vector>
#include <iostream>
#include <set>
#include <glm/gtx/string_cast.hpp>
using namespace Engine;
using namespace Object;
using namespace glm;

vec2 cross(float s, const vec2 &a)
{
    return vec2(-s * a.y, s * a.x);
}

static void resolve_collision(CollisionResolver2D::CollisionObject& lhs, CollisionResolver2D::CollisionObject& rhs, 
                              CollisionShape2D::CollisionResult result)
{
    // assert (result.penetration_distance >= 0);
    assert (lhs.body != nullptr);
    assert (rhs.body != nullptr);

    bool any_converging_velocities = false;
    float total_velocity = lhs.body->speed() + rhs.body->speed();
    float lhs_velocity_factor = lhs.body->speed() / total_velocity;
    float rhs_velocity_factor = rhs.body->speed() / total_velocity;

    for (int i = 0; i < result.intersection_point_count; i++)
    {
        auto lhs_contact = vec_3to2(lhs.transform.position()) - result.intersection_points[i];
        auto rhs_contact = vec_3to2(rhs.transform.position()) - result.intersection_points[i];

        auto relative_velocity = 
            (lhs.body->velocity() + cross(lhs.body->angular_velocity(), -lhs_contact)) -
            (rhs.body->velocity() + cross(rhs.body->angular_velocity(), -rhs_contact));
        auto velocity_along_normal = glm::dot(relative_velocity, result.normal);

        // Do not resolve if velocities are separating
        if (velocity_along_normal > 0)
            continue;
        any_converging_velocities = true;
        
        auto lhs_contact_dot_normal = glm::dot(lhs_contact, result.normal);
        auto rhs_contact_dot_normal = glm::dot(rhs_contact, result.normal);
        auto inverse_mass_part = 
            1.0f / lhs.body->mass() + 
            1.0f / rhs.body->mass() + 
            lhs_contact_dot_normal*lhs_contact_dot_normal * (1.0 / lhs.body->inertia()) + 
            rhs_contact_dot_normal*rhs_contact_dot_normal * (1.0 / rhs.body->inertia());

        // Calculate impulse
        float restitution = std::min(lhs.body->restitution(), rhs.body->restitution());
        float impulse_scalar = 
            ((1 + restitution) * velocity_along_normal)
            / inverse_mass_part;

        // Apply impulse
        auto impulse = impulse_scalar * result.normal;
        lhs.body->apply_impulse(-impulse, lhs_contact);
        rhs.body->apply_impulse(impulse, rhs_contact);

        // Calculate friction impulse
        auto tangent_velocity = relative_velocity - (result.normal * velocity_along_normal);
        if (tangent_velocity != vec2(0, 0))
        {
            auto tangent_velocity_normal = glm::normalize(tangent_velocity);
            float tangent_impulse_scalar = glm::dot(relative_velocity, tangent_velocity_normal)
                / inverse_mass_part;

            // Apply friction impulse
            auto tangent_impulse = tangent_impulse_scalar * tangent_velocity_normal * 0.05f;
            lhs.body->apply_impulse(-tangent_impulse, lhs_contact);
            rhs.body->apply_impulse(tangent_impulse, rhs_contact);
        }
    }

    // Correct intersection
    if (any_converging_velocities)
    {
        lhs.transform.translate(vec_2to3(result.normal * result.penetration_distance * lhs_velocity_factor));
        rhs.transform.translate(vec_2to3(-result.normal * result.penetration_distance * rhs_velocity_factor));
    }
}

void CollisionResolver2D::resolve(Object::World &world)
{
    // TODO: This can be cached
    std::vector<CollisionObject> collision_objects;
    world.for_each([&collision_objects](GameObject &object)
    {
        auto colliders = object.get<Collider2D>();
        if (colliders.size() == 0)
            return IteratorDecision::Continue;
        
        // Clear collision list
        for (auto &collider : colliders)
            collider->m_objects_in_collision_with.clear();
        
        auto* transform = object.first<Transform>();
        if (!transform)
            return IteratorDecision::Continue;
        
        auto *body = object.first<PhysicsBody2D>();
        collision_objects.push_back(CollisionObject {object, *transform, body});
        return IteratorDecision::Continue;
    });

    // TODO: This should be split into broad and narrow phases
    BroadPhaseCollision2D::for_each_narrow_phase_pair(collision_objects, [](
        CollisionObject &lhs, Collider2D &lhs_collider, CollisionObject &rhs, Collider2D &rhs_collider)
    {
        auto result = CollisionShape2D::check_collisions(lhs.object, lhs_collider, rhs.object, rhs_collider);
        if (result.is_colliding)
        {
            if (lhs.body != nullptr && rhs.body != nullptr)
                resolve_collision(lhs, rhs, result);

            lhs_collider.m_objects_in_collision_with.insert(&rhs.object);
            rhs_collider.m_objects_in_collision_with.insert(&lhs.object);
        }
    });
}


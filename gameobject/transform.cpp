/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "transform.hpp"
#include "gameobject.hpp"
#include "engine/physics/collision_shape_utils_2d.hpp"
#include "glm/gtx/string_cast.hpp"
using namespace Engine;
using namespace Object;
using namespace glm;

Transform::~Transform() = default;

mat4 Transform::local_transform() const
{
	if (!m_is_local_cache_dirty)
		return m_local_transform_cache;

	mat4 transform(1);
	transform = glm::translate(transform, m_position);
	transform = glm::rotate(transform, m_rotation.x, vec3(1, 0, 0));
	transform = glm::rotate(transform, m_rotation.y, vec3(0, 1, 0));
	transform = glm::rotate(transform, m_rotation.z, vec3(0, 0, 1));
	transform = glm::scale(transform, m_scale);
	m_local_transform_cache = transform;
	m_is_local_cache_dirty = false;
	return transform;
}

mat4 Transform::local_inverse_transform() const
{
	mat4 transform(1);
	transform = glm::scale(transform, 1.0f / m_scale);
	transform = glm::rotate(transform, -m_rotation.x, vec3(1, 0, 0));
	transform = glm::rotate(transform, -m_rotation.y, vec3(0, 1, 0));
	transform = glm::rotate(transform, -m_rotation.z, vec3(0, 0, 1));
	transform = glm::translate(transform, -m_position);
	return transform;
}

static mat4 local_to_global(mat4 local, const GameObject &game_object)
{
	if (!game_object.parent())
		return local;

	const auto* parent_transform = game_object.parent()->first<Transform>();
	if (!parent_transform)
		return local;

	return parent_transform->global_transform(*game_object.parent()) * local;
}

mat4 Transform::global_transform(const GameObject &game_object) const
{
	if (m_is_global_cache_dirty || m_is_local_cache_dirty)
	{
		m_global_transform_cache = local_to_global(local_transform(), game_object);
		m_is_global_cache_dirty = false;
	}

	return m_global_transform_cache;
}

mat4 Transform::global_inverse_transform(const GameObject &game_object) const
{
	return local_to_global(local_inverse_transform(), game_object);
}

Transform::Computed Transform::computed_transform() const
{
	return Computed
	{
		m_position,
		m_scale,
		m_rotation,
		local_transform(),
	};
}

Transform::Computed2D Transform::computed_transform_2d() const
{
	mat4 transform_2d(1);
	transform_2d = glm::translate(transform_2d, vec3(vec_3to2(m_position), 0));
	transform_2d = glm::rotate(transform_2d, -m_rotation.y, vec3(0, 0, 1));
	transform_2d = glm::scale(transform_2d, vec3(vec_3to2(m_scale), 0));

	return Computed2D
	{
		vec_3to2(m_position),
		vec_3to2(m_scale),
		-m_rotation.y,
		transform_2d,
	};
}

vec3 Transform::forward() const
{
	auto x = -std::sin(m_rotation.y);
	auto z = -std::cos(m_rotation.y);
	return { x, 0, z };
}

glm::vec3 Transform::left() const
{
	auto x = -std::sin(m_rotation.y + glm::radians(90.0f));
	auto z = -std::cos(m_rotation.y + glm::radians(90.0f));
	return { x, 0, z };
}

void Transform::init(GameObject &game_object)
{
    m_game_object = &game_object;
}

void Transform::on_change(bool global_change)
{
	if (!global_change)
		m_is_local_cache_dirty = true;

	if (m_is_global_cache_dirty || !m_game_object)
		return;

	m_is_global_cache_dirty = true;
	m_game_object->for_each_child([](GameObject& child)
	{
		auto* transform = child.first<Transform>();
		if (transform)
			transform->on_change(true);

		return IteratorDecision::Continue;
	});
}


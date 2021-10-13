#include "transform.hpp"
#include "gameobject.hpp"
#include <iostream>
#include <glm/gtx/transform.hpp>
using namespace Object;
using namespace glm;

Transform::~Transform()
{
}

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

static mat4 local_to_global(mat4 local, const GameObject &gameobject)
{
	if (!gameobject.parent())
		return local;

	const auto* parent_transform = gameobject.parent()->first<Transform>();
	if (!parent_transform)
		return local;

	return parent_transform->global_transform(*gameobject.parent()) * local;
}

mat4 Transform::global_transform(const GameObject &gameobject) const
{
	if (m_is_global_cache_dirty || m_is_local_cache_dirty)
	{
		m_global_transform_cache = local_to_global(local_transform(), gameobject);
		m_is_global_cache_dirty = false;
	}

	return m_global_transform_cache;
}

mat4 Transform::global_inverse_transform(const GameObject &gameobject) const
{
	return local_to_global(local_inverse_transform(), gameobject); 
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

vec3 Transform::forward() const
{
	auto x = -std::sin(m_rotation.y);
	auto z = -std::cos(m_rotation.y);
	return vec3(x, 0, z);
}

glm::vec3 Transform::left() const
{
	auto x = -std::sin(m_rotation.y + glm::radians(90.0f));
	auto z = -std::cos(m_rotation.y + glm::radians(90.0f));
	return vec3(x, 0, z);
}

void Transform::init(GameObject &gameobject)
{
	m_gameobject = &gameobject;
}

void Transform::on_change(bool global_change)
{
	if (!global_change)
		m_is_local_cache_dirty = true;

	if (m_is_global_cache_dirty || !m_gameobject)
		return;

	m_is_global_cache_dirty = true;
	m_gameobject->for_each_child([this](GameObject& child)
	{
		auto* transform = child.first<Transform>();
		if (transform)
			transform->on_change(true);

		return IteratorDecision::Continue;
	});
}

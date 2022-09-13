/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "gameobject/component.hpp"
#include "gameobject/forward.hpp"
#include "gameobject/gameobject.hpp"
#include <glm/glm.hpp>

namespace Game
{

	class InCarCamera : public Object::ComponentBase<InCarCamera>
	{
		friend Object::ComponentBase<InCarCamera>;

	public:
		void init(Object::GameObject&) override;
		void update(Object::GameObject&, float delta) override;

	private:
		InCarCamera(const InCarCamera&) = default;

		explicit InCarCamera(Object::GameObject *player)
            : m_player(player)
        {
        }

        Object::GameObject *m_player { nullptr };
		Object::Transform *m_transform { nullptr };
		Object::Transform *m_player_transform { nullptr };
		Object::PhysicsBody2D *m_player_body { nullptr };

        glm::vec2 m_last_velocity { 0, 0 };
        glm::vec3 m_head_offset { 0, 0, 0 };

	};

}


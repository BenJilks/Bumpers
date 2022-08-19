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

	class LookAtCamera : public Object::ComponentBase<LookAtCamera>
	{
		friend Object::ComponentBase<LookAtCamera>;

	public:
		virtual void init(Object::GameObject&) override;
		virtual void update(Object::GameObject&, float delta) override;

	private:
		LookAtCamera(const LookAtCamera&) = default;

		LookAtCamera(Object::GameObject *player)
            : m_player(player)
        {
        }

        Object::GameObject *m_player { nullptr };
		Object::Transform *m_transform { nullptr };
		Object::Transform *m_player_transform { nullptr };

	};

}


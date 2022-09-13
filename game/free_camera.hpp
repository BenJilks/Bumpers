/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "gameobject/component.hpp"

namespace Game
{

	class FreeCamera : public Object::ComponentBase<FreeCamera>
	{
		friend Object::ComponentBase<FreeCamera>;

	public:
		void init(Object::GameObject&) override;
		void update(Object::GameObject&, float delta) override;

	private:
		FreeCamera(const FreeCamera&) = default;
		FreeCamera() = default;

		Object::Transform* m_transform { nullptr };
		int m_last_mouse_x { 0 };
		int m_last_mouse_y { 0 };

	};

}


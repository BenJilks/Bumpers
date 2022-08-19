/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "engine/forward.hpp"
#include "gameobject/forward.hpp"
#include "post_process_renderer.hpp"
#include <vector>

namespace Engine
{

	class BlurRenderer : public PostProcessRenderer
	{
	public:
		BlurRenderer(std::shared_ptr<Shader> shader, 
            std::shared_ptr<Texture> view, glm::vec2 direction);

        virtual ~BlurRenderer();

	protected:
		virtual void bind_inputs() final;
		virtual void on_resize(int width, int height) final;

		std::shared_ptr<Texture> m_view;
		glm::vec2 m_direction;

	};

}

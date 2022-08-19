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

	class BloomRenderer : public PostProcessRenderer
	{
	public:
		BloomRenderer(std::shared_ptr<Shader> shader, 
			std::shared_ptr<Shader> blur_shader,
            std::shared_ptr<Texture> standard_view,
			std::shared_ptr<Texture> light_view);

        virtual ~BloomRenderer();
		virtual void pre_render();

	protected:
		virtual void bind_inputs() final;
		virtual void on_resize(int width, int height) final;

		std::shared_ptr<Texture> m_standard_view;
		std::shared_ptr<RenderTexture> m_blur_horizontal_view;
		std::shared_ptr<RenderTexture> m_blur_virtual_view;

	};

}

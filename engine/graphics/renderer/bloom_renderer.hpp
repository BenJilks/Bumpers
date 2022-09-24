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
			const std::shared_ptr<Shader>& blur_shader,
            std::shared_ptr<Texture> standard_view,
			const std::shared_ptr<Texture>& light_view);

        ~BloomRenderer() override;
		void pre_render() override;

	protected:
		void bind_inputs() final;
		void on_resize(int width, int height) final;

		std::shared_ptr<Texture> m_standard_view;

        std::shared_ptr<BlurRenderer> m_blur_horizontal_renderer;
		std::shared_ptr<RenderTexture> m_blur_horizontal_view;

        std::shared_ptr<BlurRenderer> m_blur_vertical_renderer;
		std::shared_ptr<RenderTexture> m_blur_vertical_view;

	};

}

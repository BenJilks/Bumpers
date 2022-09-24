/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "engine/forward.hpp"
#include "gameobject/forward.hpp"
#include "engine/graphics/renderer/renderer.hpp"
#include <vector>

namespace Engine
{

	class PostProcessRenderer : public Renderer
	{
	public:
		explicit PostProcessRenderer(std::shared_ptr<Shader> shader);
        virtual ~PostProcessRenderer();

		void on_world_updated(Object::GameObject&) final {}

	protected:
		glm::mat4 projection_matrix(int width, int height) final;
		void on_start_frame() final {}
		void on_render() final;

		virtual void bind_inputs() = 0;
		virtual void on_resize(int width, int height) = 0;

		std::shared_ptr<Mesh> m_full_screen_quad;

	};

}

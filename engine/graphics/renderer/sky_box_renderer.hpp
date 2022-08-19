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

	class SkyBoxRenderer : public Renderer
	{
	public:
		SkyBoxRenderer(std::shared_ptr<Shader> shader);
        virtual ~SkyBoxRenderer();

		virtual void on_world_updated(Object::GameObject&) final;

	protected:
		virtual glm::mat4 projection_matrix(int width, int height) final;
		virtual void on_start_frame() final;
		virtual void on_render() final;

		struct MeshRenderData
		{
			const Object::GameObject &gameobject;
			const Object::Transform &transform;
			const Object::MeshRender &mesh_render;
		};

		std::vector<MeshRenderData> m_mesh_renders;

	};

}

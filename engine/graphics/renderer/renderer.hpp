/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "engine/forward.hpp"
#include "gameobject/forward.hpp"
#include <memory>
#include <utility>
#include <vector>
#include <glm/glm.hpp>

namespace Engine
{

	class Renderer
	{
	public:
		explicit Renderer(std::shared_ptr<Shader> shader)
			: m_shader(std::move(shader))
		{
		}

		inline void set_camera(Object::GameObject &camera) { m_camera = &camera; }

		void resize_viewport(int width, int height);
		void render();

		virtual void pre_render() {}
		virtual void on_world_updated(Object::GameObject&) = 0;

	protected:
		virtual glm::mat4 projection_matrix(int width, int height) = 0;
		virtual void on_start_frame() = 0;
		virtual void on_render() = 0;

		std::shared_ptr<Shader> m_shader;
		glm::mat4 m_projection_matrix{};
		glm::mat4 m_view{};
		glm::vec3 m_camera_position{};

		inline int width() const { return m_width; }
		inline int height() const { return m_height; }

	private:
		void view_matrix(const Object::GameObject &camera);

		Object::GameObject *m_camera { nullptr };
		int m_width { 0 };
		int m_height { 0 };

	};

}

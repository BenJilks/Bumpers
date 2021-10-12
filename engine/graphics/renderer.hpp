#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "engine/forward.hpp"
#include "gameobject/forward.hpp"

namespace Engine
{

	class Renderer
	{
	public:
		Renderer(std::shared_ptr<Shader> shader, int width, int height);
        virtual ~Renderer();

		inline void set_camera(Object::GameObject &camera) { m_camera = &camera; }
		virtual void resize_viewport(int width, int height);

		void start_frame();
		virtual void render(const Object::GameObject&);

	protected:
		void view_matrix(const Object::GameObject&);

		std::shared_ptr<Shader> m_shader;
		Object::GameObject *m_camera { nullptr };

		glm::mat4 m_projection_matrix;
		glm::mat4 m_view;
		int m_width;
		int m_height;

	};

}

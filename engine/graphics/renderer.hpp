#pragma once

#include "engine/forward.hpp"
#include "gameobject/forward.hpp"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

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
		void update_lighting(const Object::GameObject&);

	protected:
		void view_matrix(const Object::GameObject&);

		struct LightData
		{
			glm::vec3 position;
			glm::vec3 color;
		};

		std::shared_ptr<Shader> m_shader;
		Object::GameObject *m_camera { nullptr };
		std::vector<LightData> m_lights;

		glm::mat4 m_projection_matrix;
		glm::mat4 m_view;
		glm::vec3 m_camera_position;
		int m_width;
		int m_height;

	};

}

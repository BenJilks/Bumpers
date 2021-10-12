#pragma once

#include <map>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <glm/glm.hpp>
#include "gameobject/forward.hpp"
#include "engine/forward.hpp"

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace Engine
{

	// GLSL wrapper
	class Shader
	{
	public:
		static std::shared_ptr<Shader> construct(const std::string &file_path);
		~Shader();

		void bind();
		void load_mvp(glm::mat4);
		void load_diffuse(int slot);
		void load_color(glm::vec4);

	private:
		GLuint m_vertex_shader;
		GLuint m_fragment_shader;
		GLuint m_program;

		GLuint m_u_mvp { 0 };
		GLuint m_u_diffuse { 0 };
		GLuint m_u_color { 0 };

		GLuint get_uniform_location(const char *name);

		bool link();
		std::string program_info_log() const;
	};

}

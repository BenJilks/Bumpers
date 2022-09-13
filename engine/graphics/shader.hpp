/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

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
		static std::shared_ptr<Shader> construct(std::istream &stream);
		~Shader();

		void bind();
		void load_matrix(const std::string &name, glm::mat4);
		void load_int(const std::string &name, int);
		void load_vec2(const std::string &name, glm::vec2);
		void load_vec3(const std::string &name, glm::vec3);
		void load_vec4(const std::string &name, glm::vec4);
		void load_float(const std::string &name, float);
		void load_bool(const std::string &name, bool);

	private:
		GLuint m_vertex_shader;
		GLuint m_fragment_shader;
		GLuint m_program;

		GLuint get_uniform_location(const std::string &name);

		bool link();
		std::string program_info_log() const;
	};

}

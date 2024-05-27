/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "engine/forward.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <string>

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace Engine {

// GLSL wrapper
class Shader {
public:
    static std::shared_ptr<Shader> construct(std::istream& stream);
    ~Shader();

    void bind();
    void load_matrix(std::string const& name, glm::mat4);
    void load_int(std::string const& name, int);
    void load_vec2(std::string const& name, glm::vec2);
    void load_vec3(std::string const& name, glm::vec3);
    void load_vec4(std::string const& name, glm::vec4);
    void load_float(std::string const& name, float);
    void load_bool(std::string const& name, bool);

private:
    GLuint m_vertex_shader;
    GLuint m_fragment_shader;
    GLuint m_program;

    GLuint get_uniform_location(std::string const& name);

    bool link();
    std::string program_info_log() const;
};

}

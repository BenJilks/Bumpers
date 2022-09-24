/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "mesh.hpp"
#include "mesh.hpp"
#include "mesh_builder.hpp"
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
using namespace Engine;
using namespace glm;

std::shared_ptr<Mesh> Mesh::construct(const MeshBuilder &builder)
{
    auto mesh = std::shared_ptr<Mesh>(new Mesh());
    glGenVertexArrays(1, &mesh->m_vao);
    glBindVertexArray(mesh->m_vao);
    
    glGenBuffers(mesh->m_vbo.size(), mesh->m_vbo.data());

    int index = 0;
    if (builder.m_verticies.size() > 0)
        mesh->bind_buffer_data<3, GL_FLOAT>(index++, builder.m_verticies);
    if (builder.m_normals.size() > 0)
        mesh->bind_buffer_data<3, GL_FLOAT>(index++, builder.m_normals);
    if (builder.m_uv01.size() > 0)
        mesh->bind_buffer_data<4, GL_FLOAT>(index++, builder.m_uv01);
    if (builder.m_cube_texture_coords.size() > 0)
        mesh->bind_buffer_data<3, GL_FLOAT>(index++, builder.m_cube_texture_coords);
    if (builder.m_tangents.size() > 0)
        mesh->bind_buffer_data<3, GL_FLOAT>(index++, builder.m_tangents);
    if (builder.m_bitangents.size() > 0)
        mesh->bind_buffer_data<3, GL_FLOAT>(index++, builder.m_bitangents);
    mesh->bind_indicies(index++, builder.m_indicies);

    if (builder.m_is_instanced)
    {
        for (int i = 0; i < index; i++)
            glVertexAttribDivisor(i, 0);
        mesh->m_instance_count = builder.m_instance_count;
    }

    glBindVertexArray(0);
    mesh->m_count = builder.m_indicies.size();
    return mesh;
}

static void vertex_attrib_pointer(int index, GLuint data_type, int size)
{
    switch (data_type)
    {
        case GL_FLOAT:
            glVertexAttribPointer(index, size, data_type, GL_FALSE, 0, 0);
            break;
        case GL_INT:
#ifdef WEBASSEMBLY
            std::cerr << "Int vertex attributes not supported on webassembly\n";
            assert(false);
#else
            glVertexAttribIPointer(index, size, data_type, 0, 0);
#endif
            break;
    }
}

template<int size, GLuint gl_type, typename T>
void Mesh::bind_buffer_data(int index, std::vector<T> data)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[index]);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    vertex_attrib_pointer(index, gl_type, size);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glEnableVertexAttribArray(index);
}

void Mesh::bind_indicies(int index, std::vector<uint32_t> indicies)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[index]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(uint32_t), indicies.data(), GL_STATIC_DRAW);
}

void Mesh::draw() const
{
    glBindVertexArray(m_vao);
    if (m_instance_count > 0)
        glDrawElementsInstanced(GL_TRIANGLES, m_count, GL_UNSIGNED_INT, nullptr, m_instance_count);
    else
        glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteBuffers(m_vbo.size(), m_vbo.data());
    glDeleteVertexArrays(1, &m_vao);
}

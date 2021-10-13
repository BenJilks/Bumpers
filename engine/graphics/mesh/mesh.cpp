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

    mesh->bind_buffer_data<3, GL_FLOAT>(0, builder.m_verticies);
    mesh->bind_buffer_data<3, GL_FLOAT>(1, builder.m_normals);
    mesh->bind_buffer_data<2, GL_FLOAT>(2, builder.m_texture_coords);
    mesh->bind_buffer_data<3, GL_FLOAT>(3, builder.m_tangents);
    mesh->bind_buffer_data<3, GL_FLOAT>(4, builder.m_bitangents);
    mesh->bind_indicies(5, builder.m_indicies);

    if (builder.m_is_instanced)
    {
        glVertexAttribDivisor(0, 0);
        glVertexAttribDivisor(1, 0);
        glVertexAttribDivisor(2, 0);
        glVertexAttribDivisor(3, 0);
        glVertexAttribDivisor(4, 0);
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
            glVertexAttribIPointer(index, size, data_type, 0, 0);
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

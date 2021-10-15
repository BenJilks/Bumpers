#include "mesh_builder.hpp"
#include "mesh.hpp"
#include <iostream>
using namespace glm;
using namespace Engine;

MeshBuilder &MeshBuilder::add_vertex(vec3 position)
{
    m_verticies.push_back(position.x);
    m_verticies.push_back(position.y);
    m_verticies.push_back(position.z);
    return *this;
}

MeshBuilder &MeshBuilder::add_normal(glm::vec3 normal)
{
    m_normals.push_back(normal.x);
    m_normals.push_back(normal.y);
    m_normals.push_back(normal.z);
    return *this;
}

MeshBuilder &MeshBuilder::add_tangent(glm::vec3 tangent)
{
    m_tangents.push_back(tangent.x);
    m_tangents.push_back(tangent.y);
    m_tangents.push_back(tangent.z);
    return *this;
}

MeshBuilder &MeshBuilder::add_bitangent(glm::vec3 bitangent)
{
    m_bitangents.push_back(bitangent.x);
    m_bitangents.push_back(bitangent.y);
    m_bitangents.push_back(bitangent.z);
    return *this;
}

MeshBuilder &MeshBuilder::add_texture_coord(vec2 texture_coord)
{
    m_texture_coords.push_back(texture_coord.x);
    m_texture_coords.push_back(1.0f - texture_coord.y);
    return *this;
}

MeshBuilder &MeshBuilder::add_cube_texture_coord(glm::vec3 texture_coord)
{
    m_cube_texture_coords.push_back(texture_coord.x);
    m_cube_texture_coords.push_back(texture_coord.y);
    m_cube_texture_coords.push_back(texture_coord.z);
    return *this;
}

MeshBuilder &MeshBuilder::add_indicies(std::vector<uint32_t> indicies)
{
    m_indicies.insert(m_indicies.end(), indicies.begin(), indicies.end());
    return *this;
}

MeshBuilder &MeshBuilder::add_builder(const MeshBuilder &other)
{
    for (auto index : other.m_indicies)
        m_indicies.push_back(index + vertex_count());

    m_verticies.insert(m_verticies.end(), other.m_verticies.begin(), other.m_verticies.end());
    m_texture_coords.insert(m_texture_coords.end(), other.m_texture_coords.begin(), other.m_texture_coords.end());
    return *this;
}

MeshBuilder &MeshBuilder::add_quad(glm::vec2 size, bool is_y_flipped)
{
    add_vertex(vec3(-size.x, -size.y, 0.0f)).add_texture_coord(vec2(0, is_y_flipped ? 1 : 0));
    add_vertex(vec3(size.x, -size.y, 0.0f)).add_texture_coord(vec2(1, is_y_flipped ? 1 : 0));
    add_vertex(vec3(size.x, size.y, 0.0f)).add_texture_coord(vec2(1, is_y_flipped ? 0 : 1));
    add_vertex(vec3(-size.x, size.y, 0.0f)).add_texture_coord(vec2(0, is_y_flipped ? 0 : 1));
    add_indicies({0, 1, 2});
    add_indicies({0, 2, 3});
    return *this;
}

MeshBuilder &MeshBuilder::add_sky_box(float size)
{
    // Front
    add_vertex(vec3(-size, -size, -size)).add_cube_texture_coord(vec3(-1, -1, -1));
    add_vertex(vec3(size, -size, -size)).add_cube_texture_coord(vec3(1, -1, -1));
    add_vertex(vec3(size, size, -size)).add_cube_texture_coord(vec3(1, 1, -1));
    add_vertex(vec3(-size, size, -size)).add_cube_texture_coord(vec3(-1, 1, -1));
    add_indicies({0, 1, 2});
    add_indicies({0, 2, 3});

    // Back
    add_vertex(vec3(-size, -size, size)).add_cube_texture_coord(vec3(-1, -1, 1));
    add_vertex(vec3(-size, size, size)).add_cube_texture_coord(vec3(-1, 1, 1));
    add_vertex(vec3(size, size, size)).add_cube_texture_coord(vec3(1, 1, 1));
    add_vertex(vec3(size, -size, size)).add_cube_texture_coord(vec3(1, -1, 1));
    add_indicies({4 + 0, 4 + 1, 4 + 2});
    add_indicies({4 + 0, 4 + 2, 4 + 3});

    // Left
    add_vertex(vec3(-size, -size, -size)).add_cube_texture_coord(vec3(-1, -1, -1));
    add_vertex(vec3(-size, size, -size)).add_cube_texture_coord(vec3(-1, 1, -1));
    add_vertex(vec3(-size, size, size)).add_cube_texture_coord(vec3(-1, 1, 1));
    add_vertex(vec3(-size, -size, size)).add_cube_texture_coord(vec3(-1, -1, 1));
    add_indicies({8 + 0, 8 + 1, 8 + 2});
    add_indicies({8 + 0, 8 + 2, 8 + 3});

    // Right
    add_vertex(vec3(size, -size, -size)).add_cube_texture_coord(vec3(1, -1, -1));
    add_vertex(vec3(size, -size, size)).add_cube_texture_coord(vec3(1, -1, 1));
    add_vertex(vec3(size, size, size)).add_cube_texture_coord(vec3(1, 1, 1));
    add_vertex(vec3(size, size, -size)).add_cube_texture_coord(vec3(1, 1, -1));
    add_indicies({12 + 0, 12 + 1, 12 + 2});
    add_indicies({12 + 0, 12 + 2, 12 + 3});

    // Top
    add_vertex(vec3(-size, size, -size)).add_cube_texture_coord(vec3(-1, 1, -1));
    add_vertex(vec3(size, size, -size)).add_cube_texture_coord(vec3(1, 1, -1));
    add_vertex(vec3(size, size, size)).add_cube_texture_coord(vec3(1, 1, 1));
    add_vertex(vec3(-size, size, size)).add_cube_texture_coord(vec3(-1, 1, 1));
    add_indicies({16 + 0, 16 + 1, 16 + 2});
    add_indicies({16 + 0, 16 + 2, 16 + 3});

    // Bottom
    add_vertex(vec3(-size, -size, -size)).add_cube_texture_coord(vec3(-1, -1, -1));
    add_vertex(vec3(-size, -size, size)).add_cube_texture_coord(vec3(-1, -1, 1));
    add_vertex(vec3(size, -size, size)).add_cube_texture_coord(vec3(1, -1, 1));
    add_vertex(vec3(size, -size, -size)).add_cube_texture_coord(vec3(1, -1, -1));
    add_indicies({20 + 0, 20 + 1, 20 + 2});
    add_indicies({20 + 0, 20 + 2, 20 + 3});

    return *this;
}

MeshBuilder &MeshBuilder::make_instanced(int count)
{
    m_is_instanced = true;
    m_instance_count = count;
    return *this;
}

bool MeshBuilder::is_empty() const
{
    return m_indicies.empty();
}

std::shared_ptr<Mesh> MeshBuilder::build() const
{
    // Verify mesh
    if (m_texture_coords.size() != 0 && m_texture_coords.size() / 2 != vertex_count())
        return nullptr;

    return Mesh::construct(*this);
}

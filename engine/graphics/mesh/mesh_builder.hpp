/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <memory>
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include "engine/forward.hpp"

namespace Engine
{

    class MeshBuilder
    {
        friend Mesh;

    public:
        MeshBuilder() = default;

        MeshBuilder &add_vertex(glm::vec3);
        MeshBuilder &add_normal(glm::vec3);
        MeshBuilder &add_tangent(glm::vec3);
        MeshBuilder &add_bitangent(glm::vec3);
        MeshBuilder &add_uv0(glm::vec2);
        MeshBuilder &add_uv01(glm::vec2, glm::vec2);
        MeshBuilder &add_cube_texture_coord(glm::vec3);
        MeshBuilder &add_indicies(std::vector<uint32_t>);

        template<size_t size>
        MeshBuilder &add_indicies(std::array<uint32_t, size> incidies)
        {
            m_indicies.insert(m_indicies.end(), incidies.begin(), incidies.end());
            return *this;
        }
        
        MeshBuilder &add_builder(const MeshBuilder&);
        MeshBuilder &add_quad(glm::vec2 size, bool is_y_flipped);
        MeshBuilder &add_sky_box(float size);
        MeshBuilder &make_instanced(int count);
        bool is_empty() const;
        
        std::shared_ptr<Mesh> build() const;

        inline int vertex_count() const { return m_verticies.size() / 3; }

    private:
        std::vector<float> m_verticies;
        std::vector<float> m_normals;
        std::vector<float> m_tangents;
        std::vector<float> m_bitangents;
        std::vector<float> m_uv01;
        std::vector<float> m_cube_texture_coords;
        std::vector<uint32_t> m_indicies;
        bool m_is_instanced { false };
        int m_instance_count { 0 };

    };

}

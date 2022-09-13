/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once 

#include "engine/forward.hpp"
#include "engine/graphics/mesh/material.hpp"
#include "component.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <utility>

namespace Object
{

    class MeshRender : public ComponentBase<MeshRender>
    {
        friend ComponentBase<MeshRender>;

    public:
        [[nodiscard]] inline const Engine::Mesh &mesh() const { return *m_mesh; }
        [[nodiscard]] inline const Engine::Renderer &renderer() const { return *m_renderer; }
        [[nodiscard]] inline const Engine::Material &material() const { return m_material; }

        inline Engine::Material &material() { return m_material; }

    private:
        MeshRender(const MeshRender&) = default;
        MeshRender(std::shared_ptr<Engine::Mesh> mesh, std::shared_ptr<Engine::Renderer> renderer, Engine::Material material)
            : m_mesh(std::move(mesh))
            , m_renderer(std::move(renderer))
            , m_material(std::move(material))
        {
        }

        std::shared_ptr<Engine::Mesh> m_mesh;
        std::shared_ptr<Engine::Renderer> m_renderer;
        Engine::Material m_material;

    };

}

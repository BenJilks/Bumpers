#pragma once 

#include "engine/forward.hpp"
#include "engine/graphics/mesh/material.hpp"
#include "component.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace Object
{

    class MeshRender : public ComponentBase<MeshRender>
    {
        friend ComponentBase<MeshRender>;

    public:
        inline const Engine::Mesh &mesh() const { return *m_mesh; }
        inline const Engine::Renderer &renderer() const { return *m_renderer; }
        inline const Engine::Material &material() const { return m_material; }

        inline Engine::Material &material() { return m_material; }

    private:
        MeshRender(const MeshRender&) = default;
        MeshRender(std::shared_ptr<Engine::Mesh> mesh, std::shared_ptr<Engine::Renderer> renderer, Engine::Material material)
            : m_mesh(mesh)
            , m_renderer(renderer)
            , m_material(material)
        {
        }

        std::shared_ptr<Engine::Mesh> m_mesh;
        std::shared_ptr<Engine::Renderer> m_renderer;
        Engine::Material m_material;

    };

}

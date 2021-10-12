#pragma once 

#include "engine/forward.hpp"
#include "component.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace Object
{

    class MeshRender : public ComponentBase<MeshRender>
    {
        friend ComponentBase<MeshRender>;

    public:
        inline bool enabled() const { return m_diffuse != nullptr; }

        inline const Engine::Mesh &mesh() const { return *m_mesh; }
        inline const Engine::Texture &diffuse() const { return *m_diffuse; }
        inline const Engine::Renderer &renderer() const { return *m_renderer; }
        inline const glm::vec4 &color() const { return m_color; }

        inline void set_color(glm::vec4 color) { m_color = color; }
        inline void set_diffuse(std::shared_ptr<Engine::Texture> diffuse) { m_diffuse = diffuse; }

    private:
        MeshRender(const MeshRender&) = default;
        MeshRender(std::shared_ptr<Engine::Mesh> mesh, std::shared_ptr<Engine::Texture> diffuse, std::shared_ptr<Engine::Renderer> renderer)
            : m_mesh(mesh)
            , m_diffuse(diffuse)
            , m_renderer(renderer)
            , m_color(1, 1, 1, 1)
        {
        }

        std::shared_ptr<Engine::Mesh> m_mesh;
        std::shared_ptr<Engine::Texture> m_diffuse;
        std::shared_ptr<Engine::Renderer> m_renderer;
        glm::vec4 m_color;

    };

}

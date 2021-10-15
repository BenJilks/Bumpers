#include "post_process_renderer.hpp"
#include "engine/graphics/mesh/mesh.hpp"
#include "engine/graphics/mesh/mesh_builder.hpp"
#include "engine/graphics/texture/texture.hpp"
#include "engine/graphics/shader.hpp"
using namespace Engine;
using namespace glm;

PostProcessRenderer::PostProcessRenderer(std::shared_ptr<Shader> shader)
    : Renderer(shader)
{
    m_full_screen_quad = MeshBuilder()
        .add_quad(vec2(1), true)
        .build();
}

PostProcessRenderer::~PostProcessRenderer()
{    
}

glm::mat4 PostProcessRenderer::projection_matrix(int width, int height)
{
    on_resize(width, height);
    return mat4(1);
}

void PostProcessRenderer::on_render()
{
    bind_inputs();
    m_full_screen_quad->draw();
}

#include "renderer.hpp"
#include "engine/graphics/shader.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include <GL/gl.h>
using namespace Engine;
using namespace Object;
using namespace glm;

void Renderer::resize_viewport(int width, int height)
{
    m_projection_matrix = projection_matrix(width, height);
    m_width = width;
    m_height = height;
}

void Renderer::view_matrix(const GameObject &camera)
{
	const auto* transform = camera.first<Transform>();
	if (!transform)
		return;

	m_view = transform->global_inverse_transform(camera);
	m_camera_position = transform->position();
}

void Renderer::render()
{
	glViewport(0, 0, m_width, m_height);

    if (m_camera)
		view_matrix(*m_camera);
	else
		m_view = mat4(1);

    m_shader->bind();
    on_start_frame();
    on_render();
}

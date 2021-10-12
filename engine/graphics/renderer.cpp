#include "renderer.hpp"
#include "mesh/mesh.hpp"
#include "texture/texture.hpp"
#include "shader.hpp"
#include "gameobject/world.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/mesh_render.hpp"
#include "gameobject/camera.hpp"
#include "gameobject/transform.hpp"
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>
#include <iostream>
using namespace Engine;
using namespace Object;
using namespace glm;

Renderer::Renderer(std::shared_ptr<Shader> shader, int width, int height)
	: m_shader(shader) 
	, m_width(width)
	, m_height(height)
{
	resize_viewport(width, height);
}

Renderer::~Renderer()
{
}

void Renderer::view_matrix(const Object::GameObject &camera)
{
	const auto* transform = camera.first<Transform>();
	if (!transform)
		return;

	m_view = transform->global_inverse_transform(camera);
}

void Renderer::start_frame()
{
	if (m_camera)
		view_matrix(*m_camera);
	else
		m_view = mat4(1);
}

void Renderer::render(const Object::GameObject &object)
{
	const auto* mesh_render = object.first<MeshRender>();
	if (!mesh_render || &mesh_render->renderer() != this || !mesh_render->enabled())
		return;

	const auto* transform = object.first<Transform>();
	if (!transform)
		return;

	auto global_transform = transform->global_transform(object);
	m_shader->bind();
	m_shader->load_diffuse(0);
	m_shader->load_mvp(m_projection_matrix * m_view * global_transform);
	m_shader->load_color(mesh_render->color());

	mesh_render->diffuse().bind(0);
	mesh_render->mesh().draw();
	Texture::unbind();
}

void Renderer::resize_viewport(int width, int height)
{
	float aspect_ratio = (float)width / (float)height;
	m_projection_matrix = glm::perspective(70.0f, aspect_ratio, 0.1f, 1000.0f);

	m_width = width;
	m_height = height;
}

#include "renderer.hpp"
#include "mesh/mesh.hpp"
#include "texture/texture.hpp"
#include "shader.hpp"
#include "gameobject/world.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/mesh_render.hpp"
#include "gameobject/light.hpp"
#include "gameobject/camera.hpp"
#include "gameobject/transform.hpp"
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>
#include <iostream>
using namespace Engine;
using namespace Object;
using namespace glm;

#define MAX_LIGHT_COUNT 10

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
	m_camera_position = transform->position();
}

void Renderer::start_frame()
{
	if (m_camera)
		view_matrix(*m_camera);
	else
		m_view = mat4(1);
	
	// Load constent uniforms
	m_shader->bind();
	auto light_count = std::min((int)m_lights.size(), MAX_LIGHT_COUNT);
	for (int i = 0; i < light_count; i++)
	{
		const LightData &light = m_lights[i];
		m_shader->load_vec3("point_lights[" + std::to_string(i) + "].position", light.position);
		m_shader->load_vec3("point_lights[" + std::to_string(i) + "].color", light.color);
	}
	m_shader->load_int("point_light_count", light_count);
	m_shader->load_vec3("camera_position", m_camera_position);

	m_lights.clear();
}

void Renderer::render(const Object::GameObject &object)
{
	const auto* mesh_render = object.first<MeshRender>();
	if (!mesh_render || &mesh_render->renderer() != this)
		return;

	const auto* transform = object.first<Transform>();
	if (!transform)
		return;

	auto global_transform = transform->global_transform(object);
	auto &material = mesh_render->material();
	m_shader->bind();
	m_shader->load_int("diffuse_map", 0);
	m_shader->load_int("normal_map", 1);
	m_shader->load_matrix("model_matrix", global_transform);
	m_shader->load_matrix("mvp", m_projection_matrix * m_view * global_transform);
	m_shader->load_vec3("color", material.color);
	m_shader->load_vec3("specular_color", material.specular_color);
	m_shader->load_vec3("emission_color", material.emission_color);
	m_shader->load_float("normal_map_strength", material.normal_map_strength);
	m_shader->load_float("specular_focus", material.specular_focus);

	if (material.diffuse_map)
		material.diffuse_map->bind(0);
	if (material.normal_map)
		material.normal_map->bind(1);
	mesh_render->mesh().draw();
	Texture::unbind(0);
	Texture::unbind(1);
}

void Renderer::update_lighting(const Object::GameObject &object)
{
	const auto* light = object.first<Light>();
	if (!light)
		return;

	const auto* transform = object.first<Transform>();
	if (!transform)
		return;

	m_lights.push_back(LightData
	{
		.position = transform->position(),
		.color = light->color(),
	});
}

void Renderer::resize_viewport(int width, int height)
{
	float aspect_ratio = (float)width / (float)height;
	m_projection_matrix = glm::perspective(70.0f, aspect_ratio, 0.1f, 100.0f);

	m_width = width;
	m_height = height;
}

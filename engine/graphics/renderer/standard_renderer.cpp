#include "standard_renderer.hpp"
#include "engine/graphics/mesh/mesh.hpp"
#include "engine/graphics/texture/texture.hpp"
#include "engine/graphics/shader.hpp"
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

StandardRenderer::StandardRenderer(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> sky_box)
	: Renderer(shader)
	, m_sky_box(sky_box)
{
}

StandardRenderer::~StandardRenderer()
{
}

void StandardRenderer::on_world_updated(Object::GameObject &world)
{
	m_mesh_renders.clear();
	m_lights.clear();

	world.for_each([&](const GameObject &gameobject)
	{
		const auto* transform = gameobject.first<Transform>();
		if (!transform)
			return IteratorDecision::Continue;

		const auto* mesh_render = gameobject.first<MeshRender>();
		if (mesh_render && &mesh_render->renderer() == this)
		{
			m_mesh_renders.push_back(MeshRenderData
			{
				.gameobject = gameobject,
				.transform = *transform,
				.mesh_render = *mesh_render,
			});
		}

		const auto* light = gameobject.first<Light>();
		if (light)
		{
			m_lights.push_back(LightData
			{
				.transform = *transform,
				.light = *light,
			});
		}

		return IteratorDecision::Continue;
	});
}

glm::mat4 StandardRenderer::projection_matrix(int width, int height)
{
	float aspect_ratio = (float)width / (float)height;
	return glm::perspective(70.0f, aspect_ratio, 0.1f, 100.0f);
}

void StandardRenderer::on_start_frame()
{
	auto light_count = std::min((int)m_lights.size(), MAX_LIGHT_COUNT);
	for (int i = 0; i < light_count; i++)
	{
		const LightData &light = m_lights[i];
		auto &position = light.transform.position();
		auto &color = light.light.color();

		// TODO: This is extremely slow
		m_shader->load_vec3("point_lights[" + std::to_string(i) + "].position", position);
		m_shader->load_vec3("point_lights[" + std::to_string(i) + "].color", color);
	}
	m_shader->load_int("point_light_count", light_count);
	m_shader->load_vec3("camera_position", m_camera_position);

	m_shader->load_int("diffuse_map", 0);
	m_shader->load_int("normal_map", 1);
	m_shader->load_int("light_map", 2);
	m_shader->load_int("sky_box", 3);
}

void StandardRenderer::on_render()
{
	for (const auto &data : m_mesh_renders)
	{
		auto global_transform = data.transform.global_transform(data.gameobject);
		auto &material = data.mesh_render.material();
		m_shader->load_matrix("model_matrix", global_transform);
		m_shader->load_matrix("mvp", m_projection_matrix * m_view * global_transform);
		m_shader->load_vec3("color", material.color);
		m_shader->load_vec3("specular_color", material.specular_color);
		m_shader->load_vec3("emission_color", material.emission_color);
		m_shader->load_float("normal_map_strength", material.normal_map_strength);
		m_shader->load_float("specular_focus", material.specular_focus);
		m_shader->load_float("metallic", material.metallic);
		m_shader->load_bool("has_light_map", material.light_map != nullptr);

		if (material.diffuse_map)
			material.diffuse_map->bind(0);
		if (material.normal_map)
			material.normal_map->bind(1);
		if (material.light_map)
			material.light_map->bind(2);
		if (m_sky_box)
		    m_sky_box->bind(3);
		data.mesh_render.mesh().draw();
		Texture::unbind(0);
		Texture::unbind(1);
		Texture::unbind(2);
		Texture::unbind(3);
	}
}

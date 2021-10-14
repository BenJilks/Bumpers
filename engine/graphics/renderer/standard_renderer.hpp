#pragma once

#include "engine/forward.hpp"
#include "gameobject/forward.hpp"
#include "engine/graphics/renderer/renderer.hpp"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace Engine
{

	class StandardRenderer : public Renderer
	{
	public:
		StandardRenderer(std::shared_ptr<Shader> shader);
        virtual ~StandardRenderer();

		virtual void on_world_updated(Object::GameObject&) final;

	protected:
		virtual glm::mat4 projection_matrix(int width, int height) final;
		virtual void on_start_frame() final;
		virtual void on_render() final;
				
		struct LightData
		{
			const Object::Transform &transform;
			const Object::Light &light;
		};

		struct MeshRenderData
		{
			const Object::GameObject &gameobject;
			const Object::Transform &transform;
			const Object::MeshRender &mesh_render;
		};

		std::vector<LightData> m_lights;
		std::vector<MeshRenderData> m_mesh_renders;

	};

}

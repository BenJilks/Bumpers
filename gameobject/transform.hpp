#pragma once

#include "component.hpp"
#include "forward.hpp"
#include <glm/glm.hpp>

namespace Object
{

	class Transform : public ComponentBase<Transform>
	{
		friend ComponentBase<Transform>;

	public:
        virtual ~Transform();
        
		struct Computed
		{
			glm::vec3 position;
			glm::vec3 scale;
            glm::vec3 rotation;
			glm::mat4 transform;
		};

		glm::mat4 local_transform() const;
		glm::mat4 local_inverse_transform() const;
		glm::mat4 global_transform(const GameObject &gameobject) const;
		glm::mat4 global_inverse_transform(const GameObject &gameobject) const;

		Computed computed_transform() const;

        inline void set_position(glm::vec3 position) { m_position = position; on_change(); }
		inline void set_scale(glm::vec3 scale) { m_scale = scale; on_change(); }
		inline void set_rotation(glm::vec3 rotation) { m_rotation = rotation; on_change(); }

		inline const glm::vec3 &position() const { return m_position; }
        inline const glm::vec3 &scale() const { return m_scale; }
		inline const glm::vec3 &rotation() const { return m_rotation; }

		inline void translate(glm::vec3 offset) { m_position += offset; on_change(); }
		inline void scale_by(glm::vec3 amount) { m_scale += amount; on_change(); }
		inline void rotate(glm::vec3 axis, float amount) { m_rotation += axis * amount; }

		glm::vec3 forward() const;
		glm::vec3 left() const;

		virtual void init(GameObject&) override;

	private:
		Transform(const Transform&) = default;
		Transform()
			: m_position(0)
			, m_scale(1)
			, m_rotation(0)
			, m_global_transform_cache(1)
			, m_local_transform_cache(1)
		{
		}

		void on_change(bool global_change = false);

		glm::vec3 m_position;
		glm::vec3 m_scale;
		glm::vec3 m_rotation;

		GameObject* m_gameobject { nullptr };
		mutable glm::mat4 m_global_transform_cache;
		mutable glm::mat4 m_local_transform_cache;
		mutable bool m_is_global_cache_dirty { true };
		mutable bool m_is_local_cache_dirty { true };

	};

}

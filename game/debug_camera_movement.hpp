#pragma once

#include "gameobject/component.hpp"

namespace Game
{

	class DebugCameraMovement : public Object::ComponentBase<DebugCameraMovement>
	{
		friend Object::ComponentBase<DebugCameraMovement>;

	public:
		virtual void init(Object::GameObject&) override;
		virtual void update(Object::GameObject&, float delta) override;

	private:
		DebugCameraMovement(const DebugCameraMovement&) = default;
		DebugCameraMovement() = default;

		Object::Transform* m_transform { nullptr };

	};

}

#pragma once
#include <glm/glm.hpp>
namespace Cle::Gfx {
	struct Light {
		enum Type {
			Point,
			Spotlight,
			Directional,
		};
		Type m_Type = Type::Point;
		float intensity = 10.0f;
		glm::vec3 color = glm::vec3(1, 1, 1);
		glm::vec3 pointTo = glm::vec3(0,0,0);
	};
}
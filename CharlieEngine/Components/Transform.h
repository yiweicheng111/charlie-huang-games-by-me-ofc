#pragma once
#include "GfxBase.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
namespace Cle::Components
{
	struct Transform
	{
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 computeMatrix();
		glm::quat orientation = glm::quat(glm::vec3(0, 0, 0));
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
	};
}
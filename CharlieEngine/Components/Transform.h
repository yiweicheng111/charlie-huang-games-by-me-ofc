#pragma once
#include "GfxBase.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
namespace Cle::Components
{
	class Transform
	{
	private:
		glm::quat orientation = glm::quat(glm::vec3(0, 0, 0));
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
	public:

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 computeMatrix();
		glm::quat getOrientation() { return orientation; }
		glm::vec3 getPosition() { return position; }
		glm::vec3 getScale() { return scale; }


		void setScale(glm::vec3 _scale) { scale = _scale; AABBdirty = true; }
		void setPosition(glm::vec3 _position) { position = _position; AABBdirty = true; }
		void setOrientation(glm::quat _rot) { orientation = _rot; AABBdirty = true; }

		bool AABBdirty = false;
	};
}
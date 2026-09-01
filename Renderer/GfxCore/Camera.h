#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

namespace Cle::Gfx
{
	class Camera
	{
	public:
		glm::vec3 Position{ 0,0,0 };
		float pitch{};
		float yaw = -glm::pi<float>() / 2;
		float fov{};
		float aspect{};
		float far = 1000.0f;
		float near = 0.1f;
		float width = 1;
		float height = 1;
		Camera() : fov(glm::radians(70.0f)), aspect(1) {}
		Camera(float fov, float aspect);
		void lookAt(glm::vec3 target);
		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjection() const;
		glm::vec3 getForward() const;
		glm::vec3 getRight() const;
		glm::vec3 getUp() const;
	private:
		glm::vec3 Forward;
		glm::mat4 View;
		glm::mat4 Projection;
	};
}
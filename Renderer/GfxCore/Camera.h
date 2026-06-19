#pragma once
#include "GfxBase.h"
namespace Cle::Gfx
{
	class Camera
	{
	public:
		glm::vec3 Position;
		float pitch{};
		float yaw = -glm::pi<float>() / 2;
		float fov{};
		float aspect{};
		float far = 10000.0f;
		float near = 0.1f;
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
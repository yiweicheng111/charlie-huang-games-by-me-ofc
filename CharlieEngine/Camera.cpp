#include "Mesh.h"
#include "Material.h"
#include "CharlieEngine/Transform.h"
#include "Camera.h"
#include <iostream>
#include "IRenderer.h"
#include "OpenGL4\OpenGLRenderer.h"
Cle::Gfx::Camera::Camera(float fov, float aspect)
{
	this->fov = fov;
	this->aspect = aspect;
	Position = { 0,0,0 };
	View = glm::lookAt(glm::vec3(0,0,0), glm::vec3(0, 0, 0) + getForward(), {0,1,0});
	Projection = glm::perspective(fov, aspect, near, far);
}

void Cle::Gfx::Camera::lookAt(const glm::vec3 target)
{
	glm::vec3 delta = target - Position;
	if (glm::length(delta) < 1e-6f) return;  

	glm::vec3 direction = glm::normalize(delta);

	yaw = glm::atan(direction.z, direction.x);
	pitch = glm::asin(glm::clamp(direction.y, -1.0f, 1.0f));
}

glm::mat4 Cle::Gfx::Camera::getProjection() const
{
	return glm::perspective(fov, aspect, near, far);
}

glm::mat4 Cle::Gfx::Camera::getViewMatrix() const
{
	return glm::lookAt(glm::vec3(0, 0, 0), getForward(), { 0,1,0 }); 
}
glm::vec3 Cle::Gfx::Camera::getForward() const
{
	return glm::normalize(glm::vec3(glm::cos(yaw)*glm::cos(pitch),glm::sin(pitch),glm::sin(yaw)*glm::cos(pitch)));
}
glm::vec3 Cle::Gfx::Camera::getRight() const
{
	return glm::normalize(glm::cross(getForward(), glm::vec3(0, 1, 0)));
}

glm::vec3 Cle::Gfx::Camera::getUp() const {
	return glm::cross(getRight(), getForward());
}

#include "Mesh.h"
#include "Material.h"
#include "Components/Transform.h"
#include "Camera.h"
#include <iostream>
#include "IRenderer.h"
#include "OpenGL4\OpenGLRenderer.h"
Cle::Gfx::Camera::Camera(float fov, float aspect)
{
	this->fov = fov;
	this->aspect = aspect;
	Position = { 0,0,0 };
	View = glm::lookAt(Position, Position + getForward(), {0,1,0});
	Projection = glm::perspective(fov, aspect, near, far);
}

void Cle::Gfx::Camera::lookAt(glm::vec3 target)
{
	View = glm::lookAt(Position, target, { 0,1,0 });
}

glm::mat4 Cle::Gfx::Camera::getProjection() const
{
	return glm::perspective(fov, aspect, near, far);
}
glm::mat4 Cle::Gfx::Camera::getViewMatrix() const
{
	return glm::lookAt(Position, Position + getForward(), { 0,1,0 });
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
glm::mat4 Cle::Components::Transform::computeMatrix()
{
	if (dirty)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model *= glm::mat4_cast(orientation);
		model = glm::scale(model, scale);
		dirty = false;
	}
	return model;

}

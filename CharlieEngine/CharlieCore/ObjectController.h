#pragma once
#include <glm/glm.hpp>
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include "../Components/Components.h"
#include "Physics.h"
/*
#include "CameraController.h"
namespace Cle
{

	class ObjectCameraController : public CameraController
	{
	public:
		entt::entity object;
		float maxSpeed = 30;
		ObjectCameraController(Cle::Gfx::Camera* Camera, GLFWwindow* Window, entt::registry* registry,entt::entity object) 
		{
			this->m_Window = Window;
			this->m_Camera = Camera;
			this->registry = registry;
			this->object = object;

			glfwGetCursorPos(m_Window, &lastX, &lastY);
			speed = 3;
		
		}
		ObjectCameraController() = default;
		virtual void Poll() override
		{

			if (m_Camera->pitch > 3.14f / 2) m_Camera->pitch = 3.13f / 2;
			if (m_Camera->pitch < -3.14f / 2) m_Camera->pitch = -3.13f / 2;
			if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_2))
			{
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				first = true;
			}
			double X;
			double Y;

			auto& transform = registry->get<Cle::Components::Transform>(object);
			auto position = transform.getPosition();
			m_Camera->Position = position;
			JPH::BodyID bodyID = registry->get<Cle::Components::PhysicsComponent>(object).ID;
			Physics1::physicssystem->GetBodyInterface().SetFriction(bodyID, 3);

			glfwGetCursorPos(m_Window, &X, &Y);
			if (first) {
				first = false;
				lastX = X;
				lastY = Y;
			}
			m_Camera->yaw += (X - lastX) * sensitivity;
			m_Camera->pitch -= (Y - lastY) * sensitivity;
			lastX = X;
			lastY = Y;
			glm::vec3 right = glm::cross(m_Camera->getForward(), { 0,1,0 });
			glm::vec3 forward = m_Camera->getForward();
			JPH::Vec3 totalV = { 0,0,0 };
			glfwGetCursorPos(m_Window, &X, &Y);
			lastX = X;
			lastY = Y;
		
		
			if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
			{
				totalV += JPH::Vec3{ forward.x,0,forward.z } * speed;
			}
			if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
			{
				totalV += JPH::Vec3{ forward.x,0,forward.z } * -speed;

			}
			if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
			{
				totalV += JPH::Vec3{ right.x,0,right.z } * -speed;

			}
			if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
			{
				totalV += JPH::Vec3{ right.x,0,right.z } * speed;
			}
			if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
			{
				Physics1::physicssystem->GetBodyInterface().AddImpulse(bodyID, {0,2000,0});
			}

			Physics1::physicssystem->GetBodyInterface().AddLinearVelocity(bodyID, totalV);
		
	
			

		}
	};
}
*/
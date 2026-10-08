#pragma once
#include "Camera.h"
#include "GLFW/glfw3.h"
#include "CameraController.h"
#include "CharlieEngine/Network.h"
namespace Cle::Editor
{
	class CharController : public CameraController
	{
	private:
		double lastX{};
		double lastY{};
		bool first = true;
	public:
		Cle::Gfx::Camera* m_Camera;
		GLFWwindow* m_Window;
		float sensitivity = 0.001f;
		float speed = 1.0f;
		CharController(Cle::Gfx::Camera* Camera, GLFWwindow* Window) : m_Camera(Camera), m_Window(Window)
		{
			glfwGetCursorPos(m_Window, &lastX, &lastY);
		}
		CharController() = default;
		virtual void Poll() override
		{
			if (m_Camera->pitch > 3.14f / 2) m_Camera->pitch = 3.13f / 2;
			if (m_Camera->pitch < -3.14f / 2) m_Camera->pitch = -3.13f / 2;

			double X;
			double Y;

			Cle::MovementPacket input{};
			auto& network = Network::getInstance();
			auto forward = m_Camera->getForward();
			glm::vec3 right = glm::cross(forward, { 0,1,0 });
			input.forward = forward;
			input.right = right;

			if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
			{
				input.vk[0] = true;
			}
			if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
			{
				input.vk[1] = true;
			}
			if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
			{
				input.vk[2] = true;
			}
			if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
			{
				input.vk[3] = true;
			}
			if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
			{
				input.vk[4] = true;
			}
			if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT))
			{
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
			}
			else
			{
				glfwGetCursorPos(m_Window, &X, &Y);
				lastX = X;
				lastY = Y;
			}
			network.sendToServer(Header{ Cle::NetworkMessage::MovementKey }, input);
		}
		

	};
}

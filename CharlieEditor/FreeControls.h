#pragma once
#include "Camera.h"
#include "GLFW/glfw3.h"
#include "CameraController.h"
namespace Cle::Editor
{
	class FreeCameraControl : CameraController
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
		FreeCameraControl(Cle::Gfx::Camera* Camera, GLFWwindow* Window) : m_Camera(Camera), m_Window(Window)
		{
			glfwGetCursorPos(m_Window, &lastX, &lastY);
		}
		FreeCameraControl() = default;
		virtual void Poll() override
		{
			if (m_Camera->pitch > 3.14f / 2) m_Camera->pitch = 3.13f / 2;
			if (m_Camera->pitch < -3.14f / 2) m_Camera->pitch = -3.13f / 2;

			double X;
			double Y;
			if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT) && !ImGui::GetIO().WantCaptureMouse)
			{
				glfwGetCursorPos(m_Window, &X, &Y);
				if (first) {
					first = false;
					lastX = X;
					lastY = Y;
				}
				m_Camera->yaw += (X - lastX)* sensitivity;
				m_Camera->pitch -= (Y - lastY) * sensitivity;
				lastX = X;
				lastY = Y;
				glm::vec3 right = glm::cross(m_Camera->getForward(), { 0,1,0 });
				if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
				{
					m_Camera->Position += speed * m_Camera->getForward();
				}
				if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
				{
					m_Camera->Position -= speed * right;
				}
				if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
				{
					m_Camera->Position -= speed * m_Camera->getForward();
				}
				if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
				{
					m_Camera->Position += speed * right;
				}
			}
			else
			{
				glfwGetCursorPos(m_Window, &X, &Y);
				lastX = X;
				lastY = Y;
			}
		}
	};
}

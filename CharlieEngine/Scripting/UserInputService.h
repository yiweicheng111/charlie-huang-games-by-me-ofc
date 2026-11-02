#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include <iostream>
namespace Cle
{
	struct UserInputService
	{
		GLFWwindow* window;
		entt::registry* registry;
		UserInputService(entt::registry* registry) {
			this->registry = registry;
			if (registry->ctx().contains<GLFWwindow*>())
			{
				window = registry->ctx().get<GLFWwindow*>();
			}
		};
		bool IsKeyDown(int keyCode) const
		{
			if (!window)
			{
				std::cout << "no window\n";
				return false;
			}
			return glfwGetKey(window, keyCode) == GLFW_PRESS;
		}
		glm::vec2 getCursorPos() const
		{
			if (!window)
			{
				std::cout << "no window\n";
				return { 0,0 };
			}
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			return glm::vec2(x, y);
		}
		bool IsMouseButtonDown() const
		{
			if (!window)
			{
				std::cout << "no window\n";
				false;
			}
			return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);
		}
	};
}
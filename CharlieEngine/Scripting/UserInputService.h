#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
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
	
	};
}
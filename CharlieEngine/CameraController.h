#pragma once
#include <glm/glm.hpp>
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include "../CharlieEngine/Components.h"
#include "imgui/imgui.h"

class CameraController
{
protected:
	double lastX{};
	double lastY{};
	bool first = true;
public:
	Cle::Gfx::Camera* m_Camera;
	GLFWwindow* m_Window;
	entt::registry* registry;
	float sensitivity = 0.001f;
	float speed = 10.0f;
	virtual void Poll() = 0;
};
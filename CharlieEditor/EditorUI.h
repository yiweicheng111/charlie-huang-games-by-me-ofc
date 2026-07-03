#pragma once
#include <entt/entt.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include "GfxBase.h"
#include "imguizmo/imguizmo.h"
#include "CharlieEngine/Transform.h"
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "CharlieEngine/Components.h"
#include <vector>
#include<iostream>
#include "World.h"
#include "GLFW/glfw3.h"

namespace Cle::Editor
{
	class EditorUI
	{
	public:
		Cle::Gfx::Pipeline m_Pipeline = Cle::Gfx::G_PIPE_LINE;
		entt::registry* m_registry = nullptr;
		ImGuiIO* io;
		bool pointerBusy = false;
		entt::entity m_Focused_Entity = entt::null;
		GLFWwindow* m_window;
		Cle::Gfx::Camera* m_camera;
		Cle::World* World;
		EditorUI(Cle::World* World, GLFWwindow* m_window, Cle::Gfx::Camera* m_camera);
		EditorUI() {};
		void DrawChildren(entt::entity parent);
		void DrawExplorer();
		void DrawProperties();
		void DrawTopBar();
		void DrawContextMenu(entt::entity e);

		void DrawGizmo(entt::entity entity);
		void Update();

	};
}

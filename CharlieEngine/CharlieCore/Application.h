#pragma once
#include "IRenderer.h"
#include "Scripting/Scripting.h"
#include <memory>
#include <glm/gtc/quaternion.hpp>
#include <entt/entt.hpp>
namespace Cle::Core
{
	class Application
	{
	public:
		Cle::Scripting::ScriptHandler* m_ScriptHandler;
		Cle::Gfx::Camera m_camera;
		entt::registry registry;
		GLFWwindow* window;
		std::shared_ptr<Cle::Renderer::IRenderer> renderer;
		virtual entt::entity CreateDebugObject(std::vector<Cle::Gfx::Vertex>& defaultVert, std::vector<unsigned int>& indices) = 0;
		virtual entt::entity CreateDebugObject(std::vector<Cle::Gfx::Vertex>& defaultVert, std::vector<unsigned int>& indices, entt::entity Parent) = 0;
		virtual void Run() = 0;
		virtual void DestroyObject(entt::entity existing) = 0;
		virtual void Render() = 0;
		virtual void Update(float dt) = 0;
		virtual entt::entity CopyObject(entt::entity existing) = 0;
	private:
		//std::unique_ptr<IWindow> window;
		bool running = true;
	};
}
#pragma once
#include "Application.h"

namespace Cle::Core
{
	class PlayApplication : public Application
	{
	public:
		Cle::Scripting::ScriptHandler m_ScriptHandler;
		Cle::Gfx::Camera m_camera;
		entt::registry registry;
		GLFWwindow* window;

		std::shared_ptr<Cle::Renderer::IRenderer> renderer;
		PlayApplication();
		virtual entt::entity CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices) override;
		virtual entt::entity CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices, entt::entity Parent) override = 0;
		virtual void Run() override;
		virtual void Render() override;
		virtual void Update(float dt) override;
	private:
		//std::unique_ptr<IWindow> window;
		bool running = true;
	};
}

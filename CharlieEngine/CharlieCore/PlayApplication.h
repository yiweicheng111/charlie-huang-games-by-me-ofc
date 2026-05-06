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
		virtual entt::entity CreateDebugObject(std::vector<Cle::Gfx::Vertex>& defaultVert, std::vector<unsigned int>& indices) override;
		virtual void Run() override;
		virtual void Render() override;
		virtual void Update(float dt) override;
	private:
		//std::unique_ptr<IWindow> window;
		bool running = true;
	};
}

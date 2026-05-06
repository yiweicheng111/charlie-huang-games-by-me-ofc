#pragma once
#include "Application.h"
#include "IRenderer.h"
#include "FreeControls.h"
#include "EditorUI.h"
namespace Cle::Editor
{
	class EditorApplication : public  Cle::Core::Application
	{
	public:
		Cle::Scripting::ScriptHandler m_ScriptHandler;
		Cle::Gfx::Camera m_camera;
		Cle::Editor::FreeCameraControls m_Controller;
		entt::registry registry;
		Cle::Editor::EditorUI m_UIHandler;
		GLFWwindow* window;
		
		std::shared_ptr<Cle::Renderer::IRenderer> renderer;
		EditorApplication();
		virtual entt::entity CreateDebugObject(std::vector<Cle::Gfx::Vertex>& defaultVert, std::vector<unsigned int>& indices) override;
		void runPointer();
		virtual void Run() override;
		virtual void Render() override;
		virtual void Update(float dt) override;
	private:
		//std::unique_ptr<IWindow> window;
		bool running = true;
	};
}

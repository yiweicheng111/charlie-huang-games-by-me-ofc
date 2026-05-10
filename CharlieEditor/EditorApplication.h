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
		Cle::Editor::FreeCameraControls m_Controller;
		Cle::Editor::EditorUI m_UIHandler;

		std::shared_ptr<Cle::Renderer::IRenderer> renderer;
		~EditorApplication();
		EditorApplication();
		virtual entt::entity CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices) override;
		virtual entt::entity CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices,entt::entity Parent) override;
		virtual entt::entity CopyObject(entt::entity existing) override;
		virtual void DestroyObject(entt::entity existing) override;
		virtual void AudioPass() override;
		void updateAABBS();
		virtual void updateBoundingSpheres() override;

		void runHotKey();
		void runPointer();
		virtual void Run() override;
		virtual void Render() override;
		virtual void Update(float dt) override;
	private:
		//std::unique_ptr<IWindow> window;
		bool running = true;
	};
}

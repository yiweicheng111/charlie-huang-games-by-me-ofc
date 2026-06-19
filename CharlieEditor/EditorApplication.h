#pragma once
#include "Application.h"
#include "IRenderer.h"
#include "FreeControls.h"
#include "EditorUI.h"
#include "Physics.h"
#include "ObjectController.h"
namespace Cle::Editor
{
	class EditorApplication : public  Cle::Core::Application
	{
	public:
		entt::entity player;
		Cle::Scripting::ScriptHandler m_ScriptHandler;
	    Cle::Editor::FreeCameraControl m_Controller;
		//Cle::ObjectCameraController m_Controller;
		Cle::Editor::EditorUI m_UIHandler;
		std::shared_ptr<Cle::Renderer::IRenderer> renderer;
		~EditorApplication();
		EditorApplication();
	
		virtual void AudioPass() override;
		void updateAABBS();
		virtual void updateBoundingSpheres() override;
		virtual void connectServer(int port, std::string ip) override;
		virtual void listenServer() override;


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

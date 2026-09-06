#pragma once
#include "Application.h"
#include "IRenderer.h"
#include "FreeControls.h"
#include "EditorUI.h"
#include "Physics.h"
#include "ObjectController.h"
#include "FakeCharController.h"
namespace Cle::Editor
{
	class EditorApplication : public  Cle::Core::Application
	{
	public:
		bool mapLoading = false;
		//Cle::Scripting::ScriptHandler m_ScriptHandler;
	    std::unique_ptr<Cle::CameraController> m_Controller;
		//Cle::ObjectCameraController m_Controller;
		Cle::Editor::EditorUI m_UIHandler;
		~EditorApplication();
		EditorApplication();
	
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

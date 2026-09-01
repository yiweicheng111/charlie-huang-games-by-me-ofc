#pragma once
#include "Application.h"
#include "IRenderer.h"
namespace Cle
{
	class RunnableApplication : public  Cle::Core::Application
	{
	public:
		bool mapLoading = false;
		Cle::Scripting::ScriptHandler m_ScriptHandler;

		~RunnableApplication();
		RunnableApplication();

		virtual void AudioPass() override;
		virtual void updateBoundingSpheres() override;

		virtual void Run() override;
		virtual void Render() override;
		virtual void Update(float dt) override;
	private:
		//std::unique_ptr<IWindow> window;
		bool running = true;
	};
}

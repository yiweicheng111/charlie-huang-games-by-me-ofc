/*
#pragma once
#include "Application.h"
#include "IRenderer.h"
namespace Cle::Core
{
	class ReleaseApp : public  Cle::Core::Application
	{

	public:
		Cle::Scripting::ScriptHandler m_ScriptHandler;

		std::shared_ptr<Cle::Renderer::IRenderer> renderer;
		~ReleaseApp();
		ReleaseApp();
		virtual entt::entity CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices) override;
		virtual entt::entity CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices, entt::entity Parent) override;
		virtual entt::entity CopyObject(entt::entity existing) override;
		virtual void DestroyObject(entt::entity existing) override;
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
*/
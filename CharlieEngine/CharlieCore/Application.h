#pragma once
#include "IRenderer.h"
#include "Scripting/Scripting.h"
#include <memory>
#include "Mesh.h"
#include <glm/gtc/quaternion.hpp>
#include <entt/entt.hpp>
#include "Audio/AudioEngine.h"
namespace Cle::Core
{
	class Application
	{
	public:
		Cle::Scripting::ScriptHandler* m_ScriptHandler;
		Cle::Gfx::Camera m_camera;
		entt::registry registry;
		GLFWwindow* window;
		ma_engine audio_engine;

		std::shared_ptr<Cle::Renderer::IRenderer> renderer;
		virtual entt::entity CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices) = 0;
		virtual entt::entity CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices, entt::entity Parent) = 0;
		virtual entt::entity CreateDebugObject(const Cle::Gfx::GenericMesh& GMesh) = 0;

		virtual void Run() = 0;
		virtual void DestroyObject(entt::entity existing) = 0;
		virtual void AudioPass() = 0;
		virtual void Render() = 0;
		virtual void Update(float dt) = 0;
		virtual void updateBoundingSpheres() = 0;
		virtual entt::entity CopyObject(entt::entity existing) = 0;
		double Random();
	private:
		//std::unique_ptr<IWindow> window;
		bool running = true;
	};
}
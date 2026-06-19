#pragma once
#include "IRenderer.h"
#include "Scripting/Scripting.h"
#include <memory>
#include "Mesh.h"
#include <glm/gtc/quaternion.hpp>
#include <entt/entt.hpp>
#include "Audio/AudioEngine.h"
#include "World.h"
#include "enet/enet.h"
namespace Cle::Core
{
	class Application
	{
	public:
		ENetHost* client;
		ENetPeer* server;


		Cle::Scripting::ScriptHandler* m_ScriptHandler;
		Cle::Gfx::Camera m_camera;
		entt::registry registry;
		GLFWwindow* window;
		ma_engine audio_engine;
		Cle::World World;
		std::shared_ptr<Cle::Renderer::IRenderer> renderer;
		virtual void connectServer(int port, std::string ip) = 0;
		virtual void listenServer() = 0;
		virtual void Run() = 0;
		virtual void AudioPass() = 0;
		virtual void Render() = 0;
		virtual void Update(float dt) = 0;
		virtual void updateBoundingSpheres() = 0;
	private:
		//std::unique_ptr<IWindow> window;
		bool running = true;
	};
}
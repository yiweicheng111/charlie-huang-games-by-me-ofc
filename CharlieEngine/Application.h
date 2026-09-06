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
#include "Network.h"
#include "gameIO.h"

namespace Cle::Core
{
	class Application
	{
	public:
		Network* m_network;
		Cle::Gfx::Camera owned_camera;
		Cle::Gfx::Camera* m_camera;
		entt::registry registry;
		GLFWwindow* window;
		ma_engine audio_engine;
		std::unique_ptr<Cle::World> World;
		std::unique_ptr<Cle::Renderer::IRenderer> renderer;
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
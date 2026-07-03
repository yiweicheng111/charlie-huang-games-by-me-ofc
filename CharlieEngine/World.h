#pragma once
#include <entt/entt.hpp>
#include "Material.h"
#include "Mesh.h"
#include <functional>
#include "IRenderer.h"
namespace Cle {
	class World {
	public:
		bool worldLoading = false;

		entt::registry* registry;
		
		Cle::Renderer::IRenderer& renderer;
		World(entt::registry* registry, Cle::Renderer::IRenderer& renderer) : registry(registry), renderer(renderer) {
			registry->on_destroy<entt::entity>().connect<&Cle::World::DestroyObject>(*this);

		}
		entt::entity CreateDebugObject(std::shared_ptr<Cle::GenericMesh> GMesh);
		entt::entity CopyObject(entt::entity existing);
		void Snapshot(std::string path);
		void LoadFile(std::string path);
		std::function<void(void)> deleteObjectCallback;
		void DestroyObject(entt::registry& registry, entt::entity existing);
	};
}
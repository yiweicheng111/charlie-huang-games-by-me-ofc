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
		std::function<void(void)> deleteObjectCallback;
		World(entt::registry* registry, Cle::Renderer::IRenderer& renderer) : registry(registry), renderer(renderer) {}
		entt::entity CreateDebugObject(std::shared_ptr<Cle::GenericMesh> GMesh);
		entt::entity CopyObject(entt::entity existing);
		void Snapshot(std::string path);
		void LoadFile(std::string path);
		void DestroyObject(entt::entity existing);
	};
}
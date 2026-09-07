#pragma once
#include <entt/entt.hpp>
#include "Material.h"
#include "Mesh.h"
#include <functional>
#include "IRenderer.h"
#include "shared.h"
#include "Scripting/Scripting.h"
#include "Reflection.h"
namespace Cle {
	class World {
	public:
		bool worldLoading = false;

		entt::registry* registry;
		
		Cle::Renderer::IRenderer& renderer;
		World(entt::registry* registry, Cle::Renderer::IRenderer& renderer) : registry(registry), renderer(renderer) {
			registry->on_destroy<std::shared_ptr<Cle::GenericMesh>>().connect<&Cle::Renderer::IRenderer::onDeleteFunction>(&renderer);

			registry->on_destroy<Cle::Components::TreeInfo>().connect<&Cle::World::DestroyObject>(*this);
			using namespace Cle::Components;
			Scene = registry->create(); registry->emplace<TreeInfo>(Scene);  registry->emplace<Name>(Scene,"Scene");
			Server = registry->create(); registry->emplace<TreeInfo>(Server); registry->emplace<Name>(Server, "Server");
			Client = registry->create(); registry->emplace<TreeInfo>(Client); registry->emplace<Name>(Client, "Client");
			Replicated = registry->create(); registry->emplace<TreeInfo>(Replicated); registry->emplace<Name>(Replicated, "Replicated");

			Cle::ScriptHandler::getInstance().setVariables(this, registry);

			Cle::RegisterReflection();

		}
		entt::entity CreateDebugObject(std::shared_ptr<Cle::GenericMesh> GMesh);
		entt::entity CreateDebugObject();

		entt::entity CopyObject(entt::entity existing);
		std::vector<entt::entity> addModelToScene(const std::string& path);
	
		void Snapshot(std::string path);
		void LoadFile(std::string path);
		std::function<void(void)> deleteObjectCallback;
		void DestroyObject(entt::registry& registry, entt::entity existing);

		entt::entity Scene;
		entt::entity Server;
		entt::entity Client;
		entt::entity Replicated;

	};
}
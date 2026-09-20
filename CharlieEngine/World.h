#pragma once
#include <entt/entt.hpp>
#include "Material.h"
#include "Mesh.h"
#include <functional>
#include "shared.h"
#include "Scripting/Scripting.h"
#include "Reflection.h"
#include "gameIO.h"
using namespace Cle::Components;
namespace Cle {
	
	class World {
	public:
		bool worldLoading = false;

		entt::registry* registry;
		World() = default;
		World(entt::registry* registry) : registry(registry) {
		

			registry->on_destroy<Cle::Components::TreeInfo>().connect<&Cle::World::DestroyObject>(*this);
			registry->on_construct < Cle::Components::Name > ().connect<&Cle::World::OnConstructed>(*this);
			using namespace Cle::Components;
			Scene = registry->create(); registry->emplace<TreeInfo>(Scene);  registry->emplace<Name>(Scene, "Scene");  registry->emplace_or_replace<SystemType>(Scene, SystemType{ SystemType::Scene });
			Server = registry->create(); registry->emplace<TreeInfo>(Server); registry->emplace<Name>(Server, "Server");  registry->emplace_or_replace<SystemType>(Server, SystemType{ SystemType::Server });
			Client = registry->create(); registry->emplace<TreeInfo>(Client); registry->emplace<Name>(Client, "Client");  registry->emplace_or_replace<SystemType>(Client, SystemType{ SystemType::Client });
			Replicated = registry->create(); registry->emplace<TreeInfo>(Replicated); registry->emplace<Name>(Replicated, "Replicated");  registry->emplace_or_replace<SystemType>(Replicated, SystemType{ SystemType::Replication });
			//Lighting = registry->create(); registry->emplace<TreeInfo>(Replicated); registry->emplace<Name>(Lighting, "Lighting"); registry->emplace<Cle::Lighting>(Lighting,Cle::Lighting::getInstance());
			Cle::ScriptHandler::getInstance().setVariables(this, registry);

			Cle::RegisterReflection(registry);
			/*
			Cle::gameIO::getInstance().onLoaded = [this]() {
				Cle::ScriptHandler::getInstance().setVariables(this, this->registry);
				};*/
		}
		void OnConstructed(entt::registry& registry, entt::entity entity)
		{
			registry.get_or_emplace < Cle::Components::TreeInfo >(entity);
			if (!registry.any_of< SystemType>(entity))
			{
				registry.emplace< SystemType>(entity, SystemType{ SystemType::None });
			}
		}
		entt::entity CreateDebugObject(Cle::GenericMesh GMesh);
		entt::entity CreateDebugObject();
		
		entt::entity CopyObject(entt::entity existing);
		std::vector<entt::entity> addModelToScene(const std::string& path);
	
		void Snapshot(std::string path);
		void LoadFile(std::string path);
		std::function<void(void)> deleteObjectCallback;
		void DestroyObject(entt::registry& registry, entt::entity existing);
		void onSceneLoaded();
		entt::entity getTopParent(entt::entity e);
		int getVisibility(entt::entity e);
		entt::entity Scene;
		entt::entity Server;
		entt::entity Client;
		entt::entity Replicated;
		entt::entity Lighting;

	};
}
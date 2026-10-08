#pragma once
#include <entt/entt.hpp>
#include "Material.h"
#include "Mesh.h"
#include <functional>
#include "shared.h"
#include "Scripting/Scripting.h"
#include "Reflection.h"
#include "gameIO.h"
#include "reactphysics3d/reactphysics3d.h"

using namespace Cle::Components;
namespace Cle {
	
	class World {
	public:
		bool worldLoading = false;
		reactphysics3d::PhysicsCommon physicsCommon;
		reactphysics3d::PhysicsWorld* physicsWorld;
		entt::registry* registry;
		World() = default;
		World(entt::registry* registry);
		void OnConstructed(entt::registry& registry, entt::entity entity)
		{
			registry.get_or_emplace < Cle::Components::TreeInfo >(entity);
			if (!registry.any_of< SystemType>(entity))
			{
				registry.emplace< SystemType>(entity, SystemType{ SystemType::None });
			}
		}
		void run();
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
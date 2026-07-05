#pragma once
#include "enet/enet.h"
#include "entt/entt.hpp"
#include "shared.h"
#include "CharlieEngine/Transform.h"
#include <unordered_set>
namespace Cle
{
	class Server
	{
	public:
		ENetAddress address;
		ENetHost* host;
		entt::registry registry;
		bool running = true;
		std::unordered_set<entt::entity> dirtyTransforms;
		std::unordered_set<entt::entity> dirtyMeshes;

		~Server()
		{
			enet_host_destroy(host);
		}
		Server(int port);
		void updateTransform(entt::registry& registry,entt::entity entity);
		void updateMesh(entt::registry& registry, entt::entity entity);
		void sendDirtyTransforms(ENetEvent& event);
		void onJoin(ENetPeer* peer);
		void run();
		
	};
}
#pragma once
#include "enet/enet.h"
#include "entt/entt.hpp"
#include "shared.h"
#include "CharlieEngine/Transform.h"
#include <unordered_set>
#include "reactphysics3d/reactphysics3d.h"

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
		reactphysics3d::PhysicsCommon physicsCommon;
		reactphysics3d::PhysicsWorld* physicsWorld;

		~Server()
		{
			enet_host_destroy(host);
		}
		Server(int port);
		void updateTransform(entt::registry& registry,entt::entity entity);
		void updateMesh(entt::registry& registry, entt::entity entity);
		void sendDirtyTransforms();
		void listenToClient();
		template <typename data>
		void sendGlobalData(int dataType, data Data)
		{
			std::ostringstream oss(std::ios::binary);
			{
				cereal::BinaryOutputArchive ar(oss);
				ar(Cle::Header{ dataType });
				ar(Data);
				ENetPacket* packet = enet_packet_create(oss.str().data(), oss.str().size(), ENET_PACKET_FLAG_RELIABLE);
				enet_host_broadcast(host, 0, packet);
			}
		}
		void stepPhysics();
		void onJoin(ENetPeer* peer);
		void run();
		
	};
}
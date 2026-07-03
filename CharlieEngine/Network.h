#pragma once
#include "enet/enet.h"
#include <string>
#include <iostream>
#include "Components.h"
namespace Cle
{
	struct Network
	{
		ENetHost* client = nullptr;
		ENetPeer* server = nullptr;
		entt::registry* registry = nullptr;
		std::function<void()> onSceneLoaded;
		std::unordered_map< entt::entity, unsigned int> entitytonetworkID;
		~Network()
		{
			if (client) enet_host_destroy(client);
			enet_deinitialize();
		}
		Network() = default;
		Network(entt::registry* r) : registry(r) {}
		void connectServer(int port, std::string ip);
		void poll();
	};
}
#pragma once
#include "enet/enet.h"
#include "entt/entt.hpp"
#include "shared.h"
#include "Components/Transform.h"
namespace Cle
{
	class Server
	{
	public:
		ENetAddress address;
		ENetHost* host;

		entt::registry registry;
		bool running = true;
		~Server()
		{
			enet_host_destroy(host);
		}
		Server(int port);
		void loadGame(std::string path);
		void Broadcast();
		
	};
}
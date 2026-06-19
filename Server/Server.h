#pragma once
#include "enet/enet.h"
#include "packet.h"

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
		void Broadcast();
		void BroadcastTransform(ENetPeer* peer, int networkID);
		void BroadcastMesh(ENetPeer* peer, int networkID);
	};
}
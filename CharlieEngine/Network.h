#pragma once
#include "enet/enet.h"
#include <string>
#include <iostream>
#include "Components.h"
#include "shared.h"
namespace Cle
{
	class Network
	{
	private:
		Network() = default;
		Network(entt::registry* r) : registry(r) {}
		

	public:
		static Network& getInstance()
		{
			static Network instance;
			return instance;
		}
		static void setRegistry(entt::registry* registry)
		{
			getInstance().registry = registry;
		}
		Network(const Network&) = delete;
		Network& operator=(const Network&) = delete;
		ENetHost* client = nullptr;
		ENetPeer* server = nullptr;
		entt::registry* registry = nullptr;
		std::function<void()> onSceneLoaded;
		std::unordered_map< unsigned int,entt::entity> netIDtoEntity;
		~Network()
		{
			if (server)
			{
				enet_peer_disconnect(server, 0);
				enet_host_flush(client);
			}
			
			if (client) enet_host_destroy(client);
			enet_deinitialize();
		}
		void connectServer(int port, std::string ip);
		template <typename Data>
		void sendToServer(Cle::Header header, Data data)
		{ 
			if (!client || !server) return;
			std::ostringstream oss(std::ios::binary);
			cereal::BinaryOutputArchive ar(oss);
			ar(header);
			ar(data);

			ENetPacket* packet = enet_packet_create(oss.str().data(), oss.str().size(), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(server, 0, packet);
			enet_host_flush(client);
		}
		void poll();
	};
}
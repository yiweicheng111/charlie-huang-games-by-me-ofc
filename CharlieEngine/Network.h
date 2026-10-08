#pragma once
#include "enet/enet.h"
#include <string>
#include <iostream>
#include "Components.h"
#include "shared.h"
#include "World.h"
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
		Cle::World* world;
		static void setVariables(entt::registry* registry, Cle::World* world,bool server= false, int port = 0)
		{
			getInstance().registry = registry;
			getInstance().world = world;
			getInstance().isServer = server;
			if (server)
			{
				enet_initialize();
				getInstance().address.port = port;
				getInstance().address.host = ENET_HOST_ANY;
				getInstance().host = enet_host_create(&getInstance().address, 32, 2, 0, 0);
			}
		}

		bool isServer = false;
		Network(const Network&) = delete;
		Network& operator=(const Network&) = delete;
		ENetHost* host = nullptr;
		ENetPeer* peer = nullptr;
		ENetAddress address;
		entt::registry* registry = nullptr;
		std::function<void()> onSceneLoaded;
		std::unordered_map< unsigned int,entt::entity> netIDtoEntity;
		~Network()
		{
			if (peer)
			{
				enet_peer_disconnect(peer, 0);
				enet_host_flush(host);
			}
			
			if (host) enet_host_destroy(host);
			enet_deinitialize();
		}
		void connectServer(int port, std::string ip);
		template <typename Data>
		void sendToServer(Cle::Header header, Data data)
		{ 
			if (!host || !peer) return;
			std::ostringstream oss(std::ios::binary);
			{
				cereal::BinaryOutputArchive ar(oss);
				ar(header);
				ar(data);
			};

			ENetPacket* packet = enet_packet_create(oss.str().data(), oss.str().size(), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, 0, packet);
			enet_host_flush(host);
		}
		template <typename Data>
		void broadCast(Cle::Header header, Data data)
		{
			if (!host) return;
			std::ostringstream oss(std::ios::binary);
			{
				cereal::BinaryOutputArchive ar(oss);
				ar(header);
				ar(data);
			};
			std::string bytes = oss.str();
			ENetPacket* packet = enet_packet_create(bytes.data(), bytes.size(),
				ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(host, 0, packet);
			enet_host_flush(host);
		}
		void poll();
	};
}
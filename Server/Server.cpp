
#include "Server.h"
#include "shared.h"
#include "CharlieCore/gameIO.h"
#include <iostream>
Cle::Server::Server(int port)
{
	std::cout<<port<<std::endl;
	enet_initialize();
	address.port = port;
	address.host = ENET_HOST_ANY;
	host = enet_host_create(&address,32,2,0,0);
	Cle::gameIO::getInstance().setRegistry(&registry);
}
void Cle::Server::Broadcast()
{
	ENetEvent event;
	while (running)
	{
		while (enet_host_service(host, &event, 1000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:

				std::stringstream ss;
				auto peer = event.peer;
				for (auto& entity : registry.view<networkID>())
				{
					auto netID = &registry.get<networkID>(entity);
					if (netID->value == -1) netID->value = (int)entity;
					auto transform = registry.try_get < Cle::Components::Transform> (entity);
				//	auto mesh = registry.try_get<MeshPacket>(entity);
					auto packet = EntityPacket{};

					packet.m_networkID = registry.get<networkID>(entity);

					if (transform) packet.transform = *transform;
				//	if (mesh) packet.mesh = *mesh;
					cereal::BinaryOutputArchive output(ss);
					output(packet);
					auto data = ss.str();
					auto enet_packet = enet_packet_create(data.data(),data.size(),ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, 0, enet_packet);
				}
			}
		}
	}

}

int main()
{
	Cle::Server s(8080);
	auto& registry = s.registry;
	Cle::gameIO::getInstance().LoadFile("D:/charlie-huang-games-by-me-ofc-main/build/world.bin");
	for (auto e : registry.view<Cle::Components::Name>())
	{
		std::cout << registry.get<Cle::Components::Name>(e).getName() << std::endl;
	}
	s.Broadcast();
	return 0;
}


#include "Server.h"
#include "shared.h"
#include "CharlieEngine/gameIO.h"
#include <iostream>
#include <fstream>
using namespace Cle;
Cle::Server::Server(int port)
{
	std::cout<<port<<std::endl;
	enet_initialize();
	address.port = port;
	address.host = ENET_HOST_ANY;
	host = enet_host_create(&address,32,2,0,0);
	Cle::gameIO::getInstance().setRegistry(&registry);
	registry.on_update<Cle::Components::Transform>().connect<&Cle::Server::updateTransform>(*this);
	registry.on_update<std::shared_ptr<Cle::GenericMesh>>().connect<&Cle::Server::updateMesh>(*this);


}
void Cle::Server::updateTransform(entt::registry& registry, entt::entity entity)
{
	dirtyTransforms.insert(entity);
}
void Cle::Server::updateMesh(entt::registry& registry, entt::entity entity)
{
	dirtyMeshes.insert(entity);
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
			{
				onJoin(event.peer);
			}
			}
		}
	}
}
void Cle::Server::onJoin(ENetPeer* peer)
{

	std::ostringstream oss(std::ios::binary);
	{
		cereal::BinaryOutputArchive ar(oss);
		std::vector<EntityPacket> packets;
		for (auto ent : registry.view<Replicated>())
		{
			EntityPacket entityp;
			if (!registry.any_of<networkID>(ent))
			{
				registry.emplace<networkID>(ent, (int)ent);
			}
			entityp.netID = registry.get<networkID>(ent);
			if (registry.any_of<Cle::Components::Transform>(ent))
			{
				entityp.transform = registry.get<Cle::Components::Transform>(ent);
			}
		
			if (registry.any_of<Cle::Components::Color>(ent))
			{
				entityp.color = registry.get<Cle::Components::Color>(ent);
			}
			if (registry.any_of<std::shared_ptr<GenericMesh>>(ent))
			{
				auto& gmesh = registry.get<std::shared_ptr<GenericMesh>>(ent);
				entityp.mesh = MeshPacket({gmesh->getModelPath(),gmesh->getMeshIndex()});
			}
			packets.push_back(entityp);
		
		}
		ar(Cle::Header{ ServerMessage::OnJoin });
		ar(packets);
		ENetPacket* packet = enet_packet_create(oss.str().data(), oss.str().size(), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
		
	}
}

int main()
{
	Cle::Server s(8080);
	auto& registry = s.registry;
	Cle::gameIO::getInstance().LoadFile("D:/charlie-huang-games-by-me-ofc-main/build/world.bin");
	for (auto e : registry.view<Cle::Components::Name>())
	{
		registry.emplace<Replicated>(e);

	}
	s.Broadcast();
	return 0;
}

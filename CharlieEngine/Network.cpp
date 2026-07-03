
#include "Network.h"
#include "shared.h"
void Cle::Network::connectServer(int port, std::string ip)
{
	enet_initialize();

	ENetAddress address;

	client = enet_host_create(NULL, 1, 2, 0, 0);
	if (!client)
	{
		std::cout << "cant create client\n";
		return;
	}
	enet_address_set_host(&address, ip.c_str());
	address.port = port;
	server = enet_host_connect(client, &address, 2, 1);
	if (!server)
	{
		std::cout << "server full\n";
		enet_host_destroy(client);
		return;
	}

	ENetEvent event;
	if (enet_host_service(client, &event, 1000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		std::cout << "connected\n";
	}
	else
	{
		std::cout << "connection timed out\n";
	}
}

void Cle::Network::poll()
{
	ENetEvent event;
	while (enet_host_service(client, &event, 0) > 0)
	{
		std::cout << "polling for server\n";
		if (!client || !server) continue;
		switch (event.type)
		{
        case ENET_EVENT_TYPE_RECEIVE:
        {
            std::string bytes(
                (char*)event.packet->data,
                event.packet->dataLength
            );

            std::istringstream iss(bytes, std::ios::binary);
            cereal::BinaryInputArchive ar(iss);

            Cle::Header header;
            ar(header);
            using namespace Cle;
            using namespace Cle::Components;
            if (header.msg == ServerMessage::OnJoin)
            {

                std::vector<EntityPacket> packets;
                ar(packets);

                registry->clear();

                for (auto& p : packets)
                {
                    entt::entity e = registry->create();

                    registry->emplace<networkID>(e, p.netID);

                    if (p.transform)
                        registry->emplace<Transform>(e, *p.transform);

                    if (p.color)
                        registry->emplace<Color>(e, *p.color);
            

                    if (p.mesh)
                    {
                        auto mesh = std::make_shared<GenericMesh>(
                            p.mesh->path,
                            p.mesh->meshIndex
                        );

                        registry->emplace<std::shared_ptr<GenericMesh>>(e, mesh);
                        
                    }
                }
            }
            if (onSceneLoaded) onSceneLoaded();
            enet_packet_destroy(event.packet);
            break;
        }
		}
      
	}
}

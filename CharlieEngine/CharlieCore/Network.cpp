
#include "Network.h"
#include "shared.h"
void Cle::Network::connectServer(int port, std::string ip)
{
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
		std::cout << "connected sucess\n";
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
			EntityPacket packet;
			std::stringstream ss;
			std::string_view bytes(
				reinterpret_cast<char*>(event.packet->data),
				event.packet->dataLength
			);
			ss << bytes;

			cereal::BinaryInputArchive input(ss);
			input(packet);

			break;
		}
	}
}

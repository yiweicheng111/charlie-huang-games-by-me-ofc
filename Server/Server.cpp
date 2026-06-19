#include "Server.h"
#include "packet.h"
#include <iostream>
Cle::Server::Server(int port)
{
	enet_initialize();
	address.port = port;
	address.host = ENET_HOST_ANY;
	host = enet_host_create(&address,32,2,0,0);
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
				std::cout << "client connect\n"; break;
			}
		}
	}
	
}
void Cle::Server::BroadcastTransform(ENetPeer* peer, int networkID)
{
}
int main()
{
	Cle::Server s(8080);

	s.Broadcast();
	return 0;
}

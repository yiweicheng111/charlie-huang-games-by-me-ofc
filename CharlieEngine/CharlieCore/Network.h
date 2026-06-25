#pragma once
#include "enet/enet.h"
#include <string>
#include <iostream>
namespace Cle
{
	struct Network
	{
		ENetHost* client = nullptr;
		ENetPeer* server = nullptr;
		void connectServer(int port, std::string ip);
		void poll();
	};
}
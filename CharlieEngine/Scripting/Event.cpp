#include "Scripting/Event.h"
void Cle::Event::Fire(sol::variadic_args arg)
{

	auto& network = Network::getInstance();		

	if (registry->any_of<networkID>(entity))
	{

		auto id = registry->get<networkID>(entity);

		if (arg.size() > 0 && arg.get_type() == sol::type::table)
		{
			auto tableData = TableToLuaTable(arg[0]);	
			auto packet = EventPacket(id.value, name, tableData);			

			if (network.isServer)
			{
				std::cout << "sent\n";

				network.broadCast(Cle::Header(Cle::NetworkMessage::Event), packet);
			}
			else
			{

				network.sendToServer(Cle::Header(Cle::NetworkMessage::Event), packet);
			}
		}

	}	

	for (auto& listener : listeners)
	{
		auto r = listener(arg);
		if (!r.valid())
		{
			sol::error e = r;
			std::cout << e.what() << std::endl;
		}
	}
}

#pragma once
#include <sol/sol.hpp>
#include <iostream>
#include <enet/enet.h>
#include <Network.h>
#include <vector>
#include "LuaValue.h"
namespace Cle
{
	struct EventPacket
	{
		unsigned int networkID;
		std::string name;
		LuaTable data;
		EventPacket() = default;
		EventPacket(unsigned int id, std::string n, const LuaTable& d) : networkID(id), name(n), data(d){}
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(networkID,name, data);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(networkID, name, data);
		}
	};
	struct Event
	{
		std::vector<sol::function> listeners;
		std::string name;
		entt::entity entity;
		entt::registry* registry;

		void Connect(sol::function call)
		{
			listeners.push_back(call);
		}
		std::vector<char> serializeEvent(const EventPacket& packet)
		{
			std::ostringstream oss(std::ios::binary);
			{
				cereal::BinaryOutputArchive ar(oss);
				ar(Cle::Header{ Cle::NetworkMessage::Event }, packet);
			}
			std::string s = oss.str();
			return std::vector<char>(s.begin(), s.end());
		}
		void FireArgs(sol::object args)
		{
			for (auto& fn : listeners)
			{
				auto result = fn(args);
				if (!result.valid())
				{
					sol::error e = result;
					std::cout << e.what() << std::endl;
				}
			}
		}
		void Fire(sol::variadic_args arg)
		{

			auto& network = Network::getInstance();					
			std::cout << "firing on entity " << (int)entity << " hasNetID=" << registry->any_of<networkID>(entity) << "\n";
			if (registry->any_of<networkID>(entity))
			{
				auto id = registry->get<networkID>(entity);

				if (arg.size() > 0 && arg.get_type() == sol::type::table)
				{
					std::cout << "sent\n";
					auto tableData = TableToLuaTable(arg[0]);
					auto packet = EventPacket(id.value, name, tableData);
					network.sendToServer(Cle::Header(Cle::NetworkMessage::Event), packet);
				}

			}
			for (const auto& listener : listeners)
			{
				
				auto data = listener(arg);
				if (!data)
				{
					sol::error e = data;
					std::cout << e.what() << std::endl;
				}
			}
		}
	};
	struct EventHolder
	{
		std::vector<Cle::Event> events;
		entt::entity entity;
		entt::registry* registry;
		EventHolder() = delete;
		EventHolder(entt::entity e, entt::registry* r) : entity(e),registry(r) {}
		Event& getOrMakeEvent(std::string name)
		{
			for (auto& i : events)
			{
				if (i.name == name)
				{
					return i;
				}
			}
			Event event;
			event.registry = registry;
			event.entity = entity;
			event.name = name;
			events.emplace_back(event);
			return events.back();
		}
	};
}
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
		std::vector<sol::main_protected_function> listeners;
		std::string name;
		entt::entity entity;
		entt::registry* registry;


		void Connect(sol::main_protected_function call)
		{
			listeners.push_back(std::move(call));
		}

		void FireArgs(sol::object args)
		{
			for (auto& fn : listeners)
			{
				auto r = fn(args);
				if (!r.valid())
				{
					sol::error e = r;
					std::cout << e.what() << std::endl;
				}
			}
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

		void Fire(sol::variadic_args arg);
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
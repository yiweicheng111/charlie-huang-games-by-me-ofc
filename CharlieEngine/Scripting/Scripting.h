#pragma once
#include <iostream>
#include <string>
#include "sol/sol.hpp"
#include <filesystem>
#include "lua/lua.h"
#include "entt/entt.hpp"
#include <cereal/cereal.hpp>
namespace Cle
{
	class World;
	struct Script
	{
		std::string path;
		sol::thread thread;
		Script() = default;
		Script(const std::string& p) : path(p) {}
		bool enabled = true;
		bool ran = false;
		sol::coroutine cor;
		double resumeTime = 0;

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(path, enabled);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(path, enabled);
		}
		bool started = false;
	};
	struct LuaEntity
	{
		entt::entity entity;
		entt::registry* registry;

	};
	struct LuaComponent
	{
		entt::entity entity;
		entt::id_type type;
		entt::registry* registry;
	};
	class ScriptHandler
	{
	public:
		std::vector<entt::entity> scriptPendingDestroy;
		sol::state lua;
		entt::registry* registry;
		void setVariables(Cle::World* world,entt::registry* registry);
		ScriptHandler(const ScriptHandler&) = delete;
		ScriptHandler& operator=(const ScriptHandler&) = delete;

		ScriptHandler(ScriptHandler&&) = delete;
		ScriptHandler& operator=(ScriptHandler&&) = delete;

		std::vector<LuaEntity> getchildren(const std::string& name);
		void run(double currentTime, double dt);
		void openLibraries();
		static ScriptHandler& getInstance()
		{
			static ScriptHandler instance;
			return instance;
		}

	private:
		Cle::World* world;

		ScriptHandler();
	};

};

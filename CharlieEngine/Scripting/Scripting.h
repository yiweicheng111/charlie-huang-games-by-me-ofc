#pragma once
#include <iostream>
#include <string>
#include "IRenderer.h"
extern "C" {
	#include "lua/lua.h"
	#include "lua/lauxlib.h"
	#include "lua/lualib.h"
}
#include <filesystem>
namespace Cle::Scripting
{
	class ScriptHandler
	{
	public:
		static ScriptHandler& getInstance()
		{
			ScriptHandler instance;
			return instance;
		}
		bool checkErrors(int r);
		void runFile(std::string path);
		ScriptHandler() {
			luaL_openlibs(m_lua_State);
		};
	private:
		lua_State* m_lua_State = luaL_newstate();
	};

}

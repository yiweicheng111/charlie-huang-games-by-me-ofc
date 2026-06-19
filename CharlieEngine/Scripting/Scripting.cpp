#include "Scripting.h"
namespace Cle::Scripting
{
	int setColor(lua_State* L) { 
		return 0;
	}
	bool ScriptHandler::checkErrors(int r)
	{
		if (r != LUA_OK)
		{
			std::string error = lua_tostring(m_lua_State, -1);
			std::cerr << error << std::endl;
			return false;
		}
		return true;
	}
	void ScriptHandler::runFile(std::string path) 
	{
		if (checkErrors(luaL_dofile(m_lua_State, path.c_str())))
		{
		}
	}
}

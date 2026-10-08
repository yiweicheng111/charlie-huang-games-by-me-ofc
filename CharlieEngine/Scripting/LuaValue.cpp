#include "LuaValue.h"
namespace Cle
{
    sol::table LuaTableToSolTable(sol::state_view lua, const LuaTable& data)

    {
        sol::table t = lua.create_table();
        for (auto& [key, value] : data)
        {
            std::visit([&](auto&& v) { t[key] = v; }, value);
        }
        return t;
    }
    LuaTable TableToLuaTable(sol::table t)
    {
     LuaTable result;
     for (auto& pair : t)
     {
         std::string key = pair.first.is<std::string>()
             ? pair.first.as<std::string>()
             : std::to_string(pair.first.as<int>());

         sol::object val = pair.second;
         if (val.is<bool>())        result[key] = val.as<bool>();
         else if (val.is<double>()) result[key] = val.as<double>();
         else if (val.is<std::string>()) result[key] = val.as<std::string>();
     }
     return result;
    }
}
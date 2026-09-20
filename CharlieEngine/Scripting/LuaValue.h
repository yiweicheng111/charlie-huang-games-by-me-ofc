#pragma once
#include "Reflection.h"
#include <cereal/types/unordered_map.hpp> 
#include <cereal/types/variant.hpp> 
using TableValue = std::variant<bool, double, std::string>;
using LuaTable = std::unordered_map<std::string, TableValue>;
namespace Cle
{
    sol::table LuaTableToSolTable(sol::state_view lua, const LuaTable& data);
    LuaTable TableToLuaTable(sol::table t);
}
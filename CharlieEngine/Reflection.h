#pragma once
#include <entt/entt.hpp>
#include "sol/sol.hpp"
#include <glm/glm.hpp>
#include <iostream>
namespace Cle
{
    struct typeInfo
    {
        entt::id_type id;
        std::string name;
    };
    
    struct SubProperty
    {
        entt::meta_type type;
        std::string name;
        entt::meta_any value;
        entt::meta_data data;
        entt::id_type componentType;
        SubProperty(const std::string& n, entt::meta_type t, entt::meta_any v, entt::meta_data d, entt::id_type c) :data(d), componentType(c), type(t), name(n), value(v) {}
    };
    struct Property
    {
        std::string parentName;
        std::vector<SubProperty> properties;
        Property(const std::string& n, std::vector<SubProperty> p) : properties(p), parentName(n) {}

    };
    extern std::vector< typeInfo> registeredComponents;
    extern std::unordered_map< entt::id_type, std::string> propertyNames;
    void RegisterReflection();
    void setLuaProperty(
        entt::registry& registry,
        entt::entity entity,
        entt::id_type componentType,
        const std::string& propertyName,
        sol::object luaValue);
    sol::object getLuaProperties(entt::registry& registry, entt::entity e, entt::id_type type, const std::string name, sol::state_view lua);
    std::vector<Property> GetEntityProperties(entt::registry& registry, entt::entity e);
    
}
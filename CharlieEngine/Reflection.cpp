#include "Reflection.h"
#include <glm/glm.hpp>
#include <entt/meta/meta.hpp>
#include "Reflection.h"
#include <entt/meta/factory.hpp>
#include <entt/meta/resolve.hpp>
#include <entt/meta/template.hpp>
#include "shared.h"
#include "Audio/AudioEngine.h"
using namespace Cle;

template <typename Component>
void registerComponent(const std::string& name)
{
    registeredComponents.push_back(typeInfo({ entt::type_hash<Component>::value(), name }));
}
void registerName(const std::string& name)
{
    propertyNames[entt::hashed_string(name.c_str()).value()] = name;
}
using namespace entt::literals;
namespace Cle
{
    std::vector< typeInfo> registeredComponents;
    std::unordered_map< entt::id_type, std::string> propertyNames;
    void RegisterReflection()
    {
        entt::meta_factory<float>{};
        entt::meta_factory<std::string>{};
        entt::meta_factory<bool>{};

        entt::meta_factory<glm::vec3>{}  
            .data<&glm::vec3::x>("x"_hs)
            .data<&glm::vec3::y>("y"_hs)
            .data<&glm::vec3::z>("z"_hs);
        registerComponent<glm::vec3>("vec3");
        registerName("x");
        registerName("y");
        registerName("z");

        entt::meta_factory<glm::vec4>{}
        .data<&glm::vec4::x>("x"_hs)
            .data<&glm::vec4::y>("y"_hs)
            .data<&glm::vec4::z>("z"_hs)
            .data<&glm::vec4::y>("w"_hs)
            ;
        registerComponent<glm::vec4>("vec4");
        registerName("x");
        registerName("y");
        registerName("z");
        registerName("w");

        entt::meta_factory<glm::quat>{}
        .data<&glm::quat::w>("w"_hs)
            .data<&glm::quat::x>("x"_hs)
            .data<&glm::quat::y>("y"_hs)
            .data<&glm::quat::z>("z"_hs);
        registerComponent<glm::quat>("quat");
        registerName("x");
        registerName("y");
        registerName("z");
        registerName("w");

        entt::meta_factory<Cle::Components::Color>{}
        .data<&Cle::Components::Color::value>("Color"_hs);
        registerComponent<Cle::Components::Color>("Color");
        registerName("Color");

        entt::meta_factory<Cle::Components::Name>{}
        .data<&Cle::Components::Name::setName, &Cle::Components::Name::getName>("name"_hs);
        registerComponent<Cle::Components::Name>("name");
        registerName("name");

        entt::meta_factory<Cle::Components::Transform>{}
        .data<&Cle::Components::Transform::setPosition, &Cle::Components::Transform::getPosition>("position"_hs)
            .data<&Cle::Components::Transform::setOrientation, &Cle::Components::Transform::getOrientation>("orientation"_hs)
            .data<&Cle::Components::Transform::setScale, &Cle::Components::Transform::getScale>("scale"_hs);
        registerComponent<Cle::Components::Transform>("transform");
        registerName("position");
        registerName("orientation");
        registerName("scale");

        entt::meta_factory<Cle::Audio::Sound>{}
        .data<&Cle::Audio::Sound::setPath, &Cle::Audio::Sound::getPath>("path"_hs)
        .data<&Cle::Audio::Sound::volume>("volume"_hs)
        .data<&Cle::Audio::Sound::setPlaying,&Cle::Audio::Sound::isPlaying>("playing"_hs)
        .data<&Cle::Audio::Sound::setTimePosition,&Cle::Audio::Sound::getTimePosition>("path"_hs)
        .data<&Cle::Audio::Sound::global>("global"_hs);
        registerComponent<Cle::Audio::Sound>("sound");

        registerName("path");
        registerName("volume");
        registerName("playing");
        registerName("position");
        registerName("global");
        std::cout << "size " << Cle::registeredComponents.size() << std::endl;

    }
    std::vector<Property> GetEntityProperties(entt::registry& registry, entt::entity e)
    {
        std::vector<Property> list;
        for (auto& compInfo : registeredComponents)
        {
            auto storage = registry.storage(compInfo.id);
            if (!storage || !storage->contains(e))
            {
                continue;

            }

            auto meta = entt::resolve(compInfo.id);
            if (!meta)

            {
                continue;

            }

            entt::meta_any instance = meta.from_void(storage->value(e));
            std::vector<SubProperty> subProperties; 
            for (auto&& [propId, propData] : meta.data())
            {

                std::string name = propertyNames.count(propId) != 0 ? propertyNames[propId] : "unknown";


                entt::meta_any value = propData.get(instance);
                entt::meta_type valueType = value.type();

                subProperties.push_back(SubProperty(name,valueType, value, propData, compInfo.id));
            }
            list.push_back(Property(std::string{ compInfo.name }, subProperties));

        }
        return list;

    }
    sol::object MetaToLua(
        entt::meta_any value,
        sol::state_view lua)
    {
        if (!value)
            return sol::make_object(lua, sol::nil);

        auto type = value.type();

        if (type == entt::resolve<float>())
            return sol::make_object(lua, value.cast<float>());

        if (type == entt::resolve<bool>())
            return sol::make_object(lua, value.cast<bool>());

        if (type == entt::resolve<std::string>())
            return sol::make_object(lua, value.cast<std::string>());

        if (type == entt::resolve<glm::vec3>())
            return sol::make_object(lua, value.cast<glm::vec3>());

        if (type == entt::resolve<glm::vec4>())
            return sol::make_object(lua, value.cast<glm::vec4>());

        if (type == entt::resolve<glm::quat>())
            return sol::make_object(lua, value.cast<glm::quat>());

        return sol::make_object(lua, sol::nil);
    }
    sol::object  getLuaProperties(entt::registry& registry, entt::entity e, entt::id_type type, const std::string name, sol::state_view lua)
    {
        auto meta = entt::resolve(type);
        auto storage = registry.storage(type);
        if (!storage || !storage->contains(e))
        {
            return sol::make_object(lua, sol::nil);
        }
        entt::meta_any instance = meta.from_void(storage->value(e));
        auto data = meta.data(entt::hashed_string(name.c_str()));
        if (!data)   return sol::make_object(lua, sol::nil);
        entt::meta_any result = data.get(instance);
        return MetaToLua(result, lua);
    }
    void setLuaProperty(
        entt::registry& registry,
        entt::entity entity,
        entt::id_type componentType,
        const std::string& propertyName,
        sol::object luaValue)
    {
        auto storage = registry.storage(componentType);

        if (!storage || !storage->contains(entity))
            return;

        auto meta = entt::resolve(componentType);

        if (!meta)
            return;

        entt::meta_any instance =
            meta.from_void(storage->value(entity));

        auto data = meta.data(
            entt::hashed_string(propertyName.c_str())
        );

        if (!data)
            return;

        if (luaValue.is<float>())
        {
            data.set(instance, luaValue.as<float>());
            return;
        }

        if (luaValue.is<bool>())
        {
            data.set(instance, luaValue.as<bool>());
            return;
        }

        if (luaValue.is<std::string>())
        {
            data.set(instance, luaValue.as<std::string>());
            return;
        }

        if (luaValue.is<glm::vec3>())
        {
            data.set(instance, luaValue.as<glm::vec3>());
            return;
        }

        if (luaValue.is<glm::vec4>())
        {
            data.set(instance, luaValue.as<glm::vec4>());
            return;
        }

        if (luaValue.is<glm::quat>())
        {
            data.set(instance, luaValue.as<glm::quat>());
            return;
        }
    }
}

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <entt/entt.hpp>
#include <iostream>
#include "nlohmann/json.hpp"
#include "Reflection.h"

using json = nlohmann::json;

inline json ReflectedToJson(entt::registry& r, entt::entity e, entt::id_type compId)
{
    json j = json::object();

    auto meta = entt::resolve(compId);
    auto* st = r.storage(compId);
    if (!meta || !st || !st->contains(e)) return j;

    entt::meta_any instance = meta.from_void(st->value(e));

    for (auto&& [propId, data] : meta.data())
    {
        auto it = Cle::propertyNames.find(propId);
        if (it == Cle::propertyNames.end()) continue;
        const std::string& key = it->second;

        entt::meta_any val = data.get(instance);
        auto t = val.type();

        if (t == entt::resolve<float>())        j[key] = val.cast<float>();
        else if (t == entt::resolve<int>())          j[key] = val.cast<int>();
        else if (t == entt::resolve<unsigned int>()) j[key] = val.cast<unsigned int>();
        else if (t == entt::resolve<bool>())         j[key] = val.cast<bool>();
        else if (t == entt::resolve<std::string>())  j[key] = val.cast<std::string>();
        else if (t == entt::resolve<glm::vec3>()) {
            auto v = val.cast<glm::vec3>();
            j[key] = json::array({ v.x, v.y, v.z });
        }
        else if (t == entt::resolve<glm::vec4>()) {
            auto v = val.cast<glm::vec4>();
            j[key] = json::array({ v.x, v.y, v.z, v.w });
        }
        else if (t == entt::resolve<glm::quat>()) {
            auto q = val.cast<glm::quat>();
            j[key] = json::array({ q.w, q.x, q.y, q.z });
        }
    }
    return j;
}

inline void ReflectedFromJson(entt::registry& r, entt::entity e, entt::id_type compId, const json& j)
{
    auto meta = entt::resolve(compId);
    auto* st = r.storage(compId);
    if (!meta || !st || !st->contains(e) || !j.is_object()) return;

    entt::meta_any instance = meta.from_void(st->value(e));

    for (auto&& [propId, data] : meta.data())
    {
        auto it = Cle::propertyNames.find(propId);
        if (it == Cle::propertyNames.end() || !j.contains(it->second)) continue;

        const json& v = j[it->second];
        auto t = data.type();

        if (t == entt::resolve<float>())        data.set(instance, v.get<float>());
        else if (t == entt::resolve<int>())          data.set(instance, v.get<int>());
        else if (t == entt::resolve<unsigned int>()) data.set(instance, v.get<unsigned int>());
        else if (t == entt::resolve<bool>())         data.set(instance, v.get<bool>());
        else if (t == entt::resolve<std::string>())  data.set(instance, v.get<std::string>());
        else if (t == entt::resolve<glm::vec3>())
            data.set(instance, glm::vec3(v[0].get<float>(), v[1].get<float>(), v[2].get<float>()));
        else if (t == entt::resolve<glm::vec4>())
            data.set(instance, glm::vec4(v[0].get<float>(), v[1].get<float>(), v[2].get<float>(), v[3].get<float>()));
        else if (t == entt::resolve<glm::quat>())
            data.set(instance, glm::quat(v[0].get<float>(), v[1].get<float>(), v[2].get<float>(), v[3].get<float>())); // w,x,y,z
    }
}
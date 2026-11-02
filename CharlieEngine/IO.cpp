
#include "Network.h"
#include "gameIO.h"
#include <fstream>
#include "Mesh.h"
#include "Material.h"
#include "CharlieEngine/Components.h"
#include "shared.h"
#include "World.h"
#include "jsonsave.h"
using namespace Cle::Components;
void Cle::gameIO::Snapshot(std::string path)
{
	json doc;
	doc["version"] = 1;
	json ents = json::array();

	for (auto e : registry->view<entt::entity>())
	{
		json ent;
		ent["id"] = entt::to_integral(e);

		json comps = json::object();
		for (auto& ci : registeredComponents)
		{
			auto* st = registry->storage(ci.id);
			if (!st || !st->contains(e)) continue;
            if (ci.jsonSave)
                comps[ci.name] = ci.jsonSave(*registry, e);       
            else
                comps[ci.name] = ReflectedToJson(*registry, e, ci.id);
		}
		ent["c"] = comps;

		if (auto* t = registry->try_get<TreeInfo>(e))
		{
			ent["tree"] = true;
			entt::entity p = t->getParent();
			if (p != entt::null && registry->valid(p))
				ent["parent"] = entt::to_integral(p);
		}

		ents.push_back(std::move(ent));
	}
	doc["entities"] = std::move(ents);

	auto bytes = json::to_msgpack(doc);
	std::ofstream f(path, std::ios::binary);
	f.write((const char*)bytes.data(), bytes.size());
}
void Cle::gameIO::LoadFile(std::string path)
{
    
    if (!registry) { loading = false; return; };
     loading = true;
    std::cout << "path loading" << path << std::endl;
    std::ifstream f(path, std::ios::binary);
    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    json doc = json::from_msgpack(bytes, true, false); 
    if (doc.is_discarded() || !doc.contains("entities")) {
        loading = false;
        return;
    }
    registry->clear();

    std::unordered_map<uint32_t, entt::entity> remap;                
    std::vector<std::pair<entt::entity, uint32_t>> pendingParents;    

    for (auto& ent : doc["entities"])
    {
        auto e = registry->create();
        remap[ent["id"].get<uint32_t>()] = e;

        if (ent.contains("c"))
        {
            for (auto& ci : registeredComponents)
            {
                if (!ent["c"].contains(ci.name) || !ci.emplace) continue;
                if (ci.jsonLoad)
                {
                    ci.jsonLoad(*registry, e, ent["c"][ci.name]);
                    continue;
                }

                if (!ci.emplace) continue;
                ci.emplace(*registry, e);
                ReflectedFromJson(*registry, e, ci.id, ent["c"][ci.name]);
            }
        }

        if (ent.value("tree", false))
        {
            registry->emplace_or_replace<TreeInfo>(e);
            if (ent.contains("parent"))
                pendingParents.emplace_back(e, ent["parent"].get<uint32_t>());
        }
    }

    for (auto& [child, savedParent] : pendingParents)
    {
        auto it = remap.find(savedParent);
        if (it == remap.end()) continue;
        if (!registry->valid(it->second) || !registry->any_of<TreeInfo>(it->second)) continue;

        auto& t = registry->get<TreeInfo>(child);
        t.setParent(child, it->second, registry);
    }
    for (auto e : registry->view<PhysicsComponent>())
        if (auto* t = registry->try_get<Transform>(e))
            registry->get<PhysicsComponent>(e).onLoad(*t);

    if (onLoaded) onLoaded();
    loading = false;
}
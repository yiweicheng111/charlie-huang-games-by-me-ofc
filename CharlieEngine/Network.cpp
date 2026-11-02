
#include "Network.h"
#include "shared.h"
#include "Scripting/Scripting.h"
#include "Scripting/Event.h"
void Cle::Network::connectServer(int port, std::string ip)
{
    enet_initialize();

    ENetAddress address;

    host = enet_host_create(NULL, 1, 2, 0, 0);
    if (!host)
    {
        std::cout << "cant create client\n";
        return;
    }
    enet_address_set_host(&address, ip.c_str());
    address.port = port;
    peer = enet_host_connect(host, &address, 2, 1);
    if (!peer)
    {
        std::cout << "server full\n";
        enet_host_destroy(host);
        return;
    }

    ENetEvent event;
    if (enet_host_service(host, &event, 1000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "connected\n";
    }
    else
    {
        std::cout << "connection timed out\n";
    }
}
void Cle::Network::addEntitesFromPacket(std::vector<EntityPacket>& packets)
{
    using namespace Cle::Components;
    std::vector<entt::entity> made;

    for (auto& p : packets)
    {
        if (netIDtoEntity.contains(p.netID.value)) continue;   

        entt::entity e = registry->create();
        registry->emplace<networkID>(e, p.netID);
        netIDtoEntity[p.netID.value] = e;
        made.push_back(e);
        if (p.script)
            registry->emplace_or_replace<Script>(e, Script(*p.script));
        if (p.systemType != SystemType::None)
            registry->emplace_or_replace<SystemType>(e, SystemType{ p.systemType });
        if (p.transform)
            registry->emplace<Transform>(e, *p.transform);
        if (p.name)
            registry->emplace_or_replace<Name>(e, *p.name);
        if (p.color)
            registry->emplace<Color>(e, *p.color);
        if (p.mesh)
        {
           // std::cout << "has mesh\n";
            registry->get_or_emplace<MaterialRef>(e);
            auto& m =registry->emplace<GenericMesh>(e, GenericMesh(p.mesh->path, p.mesh->meshIndex));
            m.uploaded = false;
            if (onEntityAdded) onEntityAdded(e);

        }
        if (p.treeinfo)
        {
            registry->get_or_emplace<TreeInfo>(e).loadingParentID = p.treeinfo->parentNetID;
        }
    }

    for (auto e : made)
    {
        auto* tree = registry->try_get<TreeInfo>(e);
        if (!tree || tree->loadingParentID == -1) continue;
        auto it = netIDtoEntity.find(tree->loadingParentID);
        if (it == netIDtoEntity.end() || !registry->valid(it->second)) continue;
        tree->setParent(e, it->second, registry);
    }
}
void Cle::Network::poll()
{
    std::vector<std::pair<int, int>> parentingQueue;
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0)
    {
        if (!host || !peer) continue;
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
        {
            using namespace Cle;
            using namespace Cle::Components;
            std::string bytes(
                (char*)event.packet->data,
                event.packet->dataLength
            );

            std::istringstream iss(bytes, std::ios::binary);
            cereal::BinaryInputArchive ar(iss);

            Cle::Header header;
            ar(header);
      
            std::vector<EntityPacket> packets;
            if (header.msg == Cle::NetworkMessage::Event)
            {
                Cle::EventPacket packet;
                ar(packet);
                auto& net = Network::getInstance();
                auto it = net.netIDtoEntity.find(packet.networkID);
                if (it == net.netIDtoEntity.end() || !registry->valid(it->second))
                {
                    return;
                }
                entt::entity entity = it->second;
                auto& holder = registry->get_or_emplace<Cle::EventHolder>(entity, entity, registry);
                auto& evt = holder.getOrMakeEvent(packet.name);

                sol::table args = LuaTableToSolTable(ScriptHandler::getInstance().lua, packet.data);
                evt.FireArgs(args);
            }
            if (header.msg == NetworkMessage::UpdateEntity)
            {
                ar(packets);
                for (auto& p : packets)
                {
                    auto it = netIDtoEntity.find(p.netID.value);
                    if (it == netIDtoEntity.end() || !registry->valid(it->second)) continue;
                    entt::entity entity = it->second;

                    if (!registry->valid(entity)) continue;
                    if (p.transform && registry->any_of<Transform>(entity))
                    {
                        registry->patch<Transform>(entity, [&](Transform& t) {
                            t.setOrientation(p.transform->getOrientation());
                            t.setPosition(p.transform->getPosition());
                            t.setScale(p.transform->getScale());
                            });

                    }
                }
            }
            else if (header.msg == NetworkMessage::DestroyEntity)
            {
                std::vector<int> destroys;
                ar(destroys);
                for (auto i : destroys)
                {
                    if (!netIDtoEntity.contains(i)) continue;

                    auto it = netIDtoEntity.find(i);
                    if (it == netIDtoEntity.end()) continue;
                    if (registry->valid(it->second)) registry->destroy(it->second);
                    netIDtoEntity.erase(it);
                }
              
            }
            else if (header.msg == NetworkMessage::CreateEntity)
            {
                std::cout << "created yes\n";

                ar(packets);
                addEntitesFromPacket(packets);

            }
            else if (header.msg == NetworkMessage::OnJoin)
            {
                ar(packets);
                std::cout << "before " << registry->storage<entt::entity>().size() << std::endl;

                registry->clear();
                netIDtoEntity.clear();
                addEntitesFromPacket(packets);              

              
            
             

                if (onSceneLoaded) onSceneLoaded();
                ScriptHandler::getInstance().setVariables(world, registry);

                std::cout << "after " << registry->storage<entt::entity>().size() << std::endl;

            }
            enet_packet_destroy(event.packet);
         
            break;
         
        }
        }

    }
}
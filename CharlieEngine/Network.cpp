
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

void Cle::Network::poll()
{

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
                    auto entity = netIDtoEntity[p.netID.value];

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
         
            else if (header.msg == NetworkMessage::OnJoin)
            {
                ar(packets);
                std::cout << "before " << registry->storage<entt::entity>().size() << std::endl;

                registry->clear();
                netIDtoEntity.clear();

              
                int count = 0;
                for (auto& p : packets)
                {
                    count++;
                    entt::entity e = registry->create();

                    registry->emplace<networkID>(e, p.netID);
                    netIDtoEntity[p.netID.value] = e;
                    if (p.script)
                    {
                        registry->emplace_or_replace<Cle::Script>(e, Script(*p.script));

                    }
                    if (p.systemType != SystemType::None)
                    {
                        registry->emplace_or_replace<SystemType>(e, SystemType{ p.systemType });
                        if (p.systemType == SystemType::Scene) std::cout << "scene found\n";
                    }
                    if (p.transform)
                        registry->emplace<Transform>(e, *p.transform);
                    if (p.name)
                    {

                        registry->emplace_or_replace<Name>(e, *p.name);
                    }

                    if (p.color)
                        registry->emplace<Color>(e, *p.color);
                    if (p.treeinfo)
                    {
                        auto& tree = registry->get_or_emplace<TreeInfo>(e);
                        tree.loadingParentID = p.treeinfo->parentNetID;
                    }


                    if (p.mesh)
                    {
                        auto mesh = GenericMesh(
                            p.mesh->path,
                            p.mesh->meshIndex
                        );

                        registry->emplace<GenericMesh>(e, mesh);

                    }
              
                }
                for (auto ent : registry->view<TreeInfo>())
                {
                    auto& tree = registry->get<TreeInfo>(ent);
                    if (tree.loadingParentID == -1) continue;
                    auto& parent = netIDtoEntity[tree.loadingParentID];
                    if (registry->any_of<Name>(parent))
                    {
                     //   std::cout << "name " << registry->get<Name>(parent).getName() << std::endl;
                    }
                    tree.setParent(ent, parent, registry);
                }

                if (onSceneLoaded) onSceneLoaded();
             
                std::cout << "after " << registry->storage<entt::entity>().size() << std::endl;

            }
            ScriptHandler::getInstance().setVariables(world, registry);
            enet_packet_destroy(event.packet);
         
            break;
         
        }
        }

    }
}
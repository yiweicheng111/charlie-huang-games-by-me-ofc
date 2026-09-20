
#include "Server.h"
#include "shared.h"
#include "CharlieEngine/gameIO.h"
#include <iostream>
#include <fstream>
#include <reactphysics3d/body/RigidBody.h>
#include "time.h"
#include "CharlieEngine/Scripting/Event.h"
using namespace Cle;
static double accumulator = 0.0;
static double dt = 1.0 / 60.0;
Cle::Server::Server(int port)
{
	std::cout << port << std::endl;
	enet_initialize();
	address.port = port;
	address.host = ENET_HOST_ANY;
	host = enet_host_create(&address, 32, 2, 0, 0);
	Cle::gameIO::getInstance().setRegistry(&registry);

	registry.on_update<Cle::Components::Transform>().connect<&Cle::Server::updateTransform>(*this);
	registry.on_update<std::shared_ptr<Cle::GenericMesh>>().connect<&Cle::Server::updateMesh>(*this);
	physicsWorld = physicsCommon.createPhysicsWorld();
	physicsWorld->setGravity(reactphysics3d::Vector3(0, -180.0f, 0));
	world = World(&registry);
	Cle::ScriptHandler::getInstance().openLibraries();
	Cle::gameIO::getInstance().onLoaded = [this]() {
		Cle::ScriptHandler::getInstance().setVariables(&this->world, &this->registry);
		};
}
void Cle::Server::updateTransform(entt::registry& registry, entt::entity entity)
{
	dirtyTransforms.insert(entity);
}
void Cle::Server::updateMesh(entt::registry& registry, entt::entity entity)
{
	dirtyMeshes.insert(entity);
}
void Cle::Server::sendDirtyTransforms()
{
	std::vector<EntityPacket> packets;

	for (const auto& entity : dirtyTransforms)
	{
		if (!registry.valid(entity) || !registry.any_of<Cle::Components::Transform>(entity)) continue;

		EntityPacket entityp;
		if (!registry.any_of<networkID>(entity))
		{
			registry.emplace<networkID>(entity, (int)entity);
		}
		entityp.netID = registry.get<networkID>(entity);

		entityp.transform = registry.get<Cle::Components::Transform>(entity);
		packets.push_back(entityp);
	}
	std::ostringstream oss(std::ios::binary);
	{
		cereal::BinaryOutputArchive ar(oss);
		ar(Cle::Header{ NetworkMessage::UpdateEntity });
		ar(packets);
	}
	ENetPacket* packet = enet_packet_create(oss.str().data(), oss.str().size(), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(host, 0, packet);
	dirtyTransforms.clear();
}
void Cle::Server::stepPhysics()
{
	
	for (auto e : registry.view<Cle::Components::PhysicsComponent>())
	{
		const auto& phy = registry.get<Cle::Components::PhysicsComponent>(e);
		auto& transform = registry.get<Cle::Components::Transform>(e);

		const auto& rtransform = phy.body->getTransform();
		auto newPos = Cle::Components::PhysicsComponent::rp3dtoglmvec3(rtransform.getPosition());
		auto newRot = Cle::Components::PhysicsComponent::rp3dtoglmquat(rtransform.getOrientation());
		if (fabs(phy.body->getLinearVelocity().y) < 1)
		{
			phy.body->applyLocalForceAtLocalPosition({ 0,100,0 },{0,0,0});
		}
		if (transform.getPosition() != newPos || transform.getOrientation() != newRot) {
			registry.patch<Cle::Components::Transform>(e, [&](Cle::Components::Transform& t) {
				t.setPosition(newPos);
				t.setOrientation(newRot);
				//std::cout << "patched\n";
				});
		}
	}


	if (host->peerCount > 0 && !dirtyTransforms.empty())
		sendDirtyTransforms();
	//std::cout << "sent\n";

}
void Cle::Server::run()
{

	ENetEvent event;
	while (running)
	{

		auto lastTime = std::chrono::steady_clock::now();
		

		while (running)
		{
			auto now = std::chrono::steady_clock::now();
			accumulator += std::chrono::duration<double>(now - lastTime).count();
			lastTime = now;
			ScriptHandler::getInstance().run(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(),0.1);
			while (enet_host_service(host, &event, 1) > 0)
			{
				if (event.type == ENET_EVENT_TYPE_CONNECT) onJoin(event.peer);
				else if (event.type == ENET_EVENT_TYPE_RECEIVE)
				{
					std::string bytes(
						(char*)event.packet->data,
						event.packet->dataLength
					);
					std::istringstream iss(bytes, std::ios::binary);
					cereal::BinaryInputArchive ar(iss);

					Cle::Header header;
					ar(header);
					Cle::EventPacket packet;						

					if (header.msg == Cle::NetworkMessage::Event)
					{
						ar(packet);
						entt::entity entity = static_cast<entt::entity>(packet.networkID);

						auto& holder = registry.get_or_emplace<Cle::EventHolder>(entity, entity, &registry);
						auto& evt = holder.getOrMakeEvent(packet.name);
						sol::table args = LuaTableToSolTable(ScriptHandler::getInstance().lua, packet.data);
						evt.FireArgs(args);
					}
				}
			}

			while (accumulator >= dt)
			{
				physicsWorld->update(dt);
				accumulator -= dt;
			}
			stepPhysics();

		}
	}
}
void Cle::Server::onJoin(ENetPeer* peer)
{

	std::ostringstream oss(std::ios::binary);
	{
		cereal::BinaryOutputArchive ar(oss);
		std::vector<EntityPacket> packets;
		int ents = 0;
		for (auto ent : registry.view<entt::entity>())
		{
			if (registry.any_of<Cle::Components::SystemType>(ent))
			{
				if (registry.get<Cle::Components::SystemType>(ent).type == Cle::Components::SystemType::Server)
				{
					continue;
				}
			}
			ents++;
			EntityPacket entityp;
			if (!registry.any_of<networkID>(ent))
			{
				registry.emplace<networkID>(ent, (int)ent);
			}
			entityp.netID = registry.get<networkID>(ent);
			if (registry.any_of<Cle::Components::Transform>(ent))
			{
				entityp.transform = registry.get<Cle::Components::Transform>(ent);
			}
			if (registry.any_of<Cle::Components::Name>(ent))
			{
				entityp.name = registry.get<Cle::Components::Name>(ent);
			}
			if (registry.any_of<Cle::Components::Color>(ent))
			{
				entityp.color = registry.get<Cle::Components::Color>(ent);
			}
			if (registry.any_of<GenericMesh>(ent))
			{
				auto& gmesh = registry.get<GenericMesh>(ent);
				entityp.mesh = MeshPacket({ gmesh.getModelPath(),gmesh.getMeshIndex() });
			}
			if (registry.any_of<Cle::Script>(ent))
			{
				auto& s = registry.get<Script>(ent);
				entityp.script = s.path;
			}
			if (registry.any_of<Components::TreeInfo>(ent))
			{
				auto parent = registry.get<Components::TreeInfo>(ent).getParent();
				if (!registry.any_of<networkID>(parent))
				{
					registry.emplace<networkID>(parent, (int)parent);
				}
				if (registry.valid(parent) && registry.any_of<networkID>(parent))
				{
					auto parentid = registry.get<networkID>(parent).value;
					
					entityp.treeinfo = TreeInfoPacket(parentid);
				}
				else
				{
					entityp.treeinfo = TreeInfoPacket(-1);
				}

			}
			if (registry.any_of<Cle::Components::SystemType>(ent))
			{
				entityp.systemType = registry.get<Cle::Components::SystemType>(ent).type;
			}
			packets.push_back(entityp);

		}
		ar(Cle::Header{ NetworkMessage::OnJoin });
		ar(packets);
		ENetPacket* packet = enet_packet_create(oss.str().data(), oss.str().size(), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
		std::cout << ents<<std::endl;

	}
}

int main()
{
	Cle::Server s(8080);
	auto& world = s.world;
	auto& registry = s.registry;
	Cle::gameIO::getInstance().LoadFile("D:/charlie-huang-games-by-me-ofc-main/build/world.bin");
	ScriptHandler::getInstance().setVariables(&world, &registry);

	for (auto e : registry.view<Cle::Components::Name>())
	{
		auto& n = registry.get<Cle::Components::Name>(e);

		if (registry.any_of<Cle::Script>(e))
		{

			if (world.getVisibility(e) == Cle::Components::SystemType::Client)
			{
				std::cout << "set\n";
				
				registry.get<Cle::Script>(e).enabled = false;
			}
		
		}

		if (registry.any_of<Components::Transform>(e))
		{
			auto& t = registry.get<Components::Transform>(e);
			auto& p = registry.emplace<Cle::Components::PhysicsComponent>(e, &s.physicsCommon, s.physicsWorld, reactphysics3d::CollisionShapeName::BOX, t);
			if (n.getName() == "michael")
			{
				p.body->setType(reactphysics3d::BodyType::DYNAMIC);
				p.body->setMass(150.0f);
				p.body->updateLocalInertiaTensorFromColliders();

			}

		}


	}
	std::cout << "done\n";
	s.run();
	return 0;
}

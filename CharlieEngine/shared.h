#pragma once
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>

#include <cereal/archives/binary.hpp>
#include "GenericMesh.h"

#include <iostream>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/glm.hpp"
#include <optional>
#include "entt/entt.hpp"
#include "CharlieEngine/Transform.h"
#include <string>
#include <functional>
#include "reactphysics3d/reactphysics3d.h"

namespace Cle::Components
{

	struct PhysicsComponent
	{
		static reactphysics3d::Vector3 glmtorp3dvec3(const glm::vec3& vec)
		{
			return reactphysics3d::Vector3(vec.x, vec.y, vec.z);
		}
		static glm::quat rp3dtoglmquat(const  reactphysics3d::Quaternion& quat)
		{
			return glm::quat(quat.w, quat.x, quat.y, quat.z);
		}
		static reactphysics3d::Quaternion glmtorp3dquat(const glm::quat& quat)
		{
			return reactphysics3d::Quaternion(quat.x, quat.y, quat.z, quat.w);
		}
		static glm::vec3 rp3dtoglmvec3(const reactphysics3d::Vector3& vec)
		{
			return glm::vec3(vec.x, vec.y, vec.z);
		}
	
		reactphysics3d::RigidBody* body;
		reactphysics3d::PhysicsCommon* physicsCommon;
		reactphysics3d::PhysicsWorld* physicsWorld;
		PhysicsComponent(reactphysics3d::PhysicsCommon* pc, reactphysics3d::PhysicsWorld* world, reactphysics3d::CollisionShapeName name, Transform t) : physicsCommon(pc), physicsWorld(world)
		{

			if (name == reactphysics3d::CollisionShapeName::BOX)
			{
				body = world->createRigidBody(reactphysics3d::Transform(glmtorp3dvec3(t.getPosition()), glmtorp3dquat(t.getOrientation())));
				body->addCollider(pc->createBoxShape(glmtorp3dvec3(t.getScale()) / 2.0f), reactphysics3d::Transform::identity());

				body->setType(reactphysics3d::BodyType::STATIC);

			}
		}
	};
	struct Color
	{

		glm::vec4 value;
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(value);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(value);
		}
	};
	class Name
	{
	private:
		std::string value{};
	public:
		bool dirty = false;
		void setName(std::string name)
		{
			dirty = true;
			value = name;
		}
		std::string getName() const
		{
			return value;
		}
		Name() : value("untitled") {}
		Name(std::string value) : value(value) {}
	
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(value);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(value);
			dirty = true;
		}

	};
	class TreeInfo
	{
	private:
		entt::entity parent = entt::null;
		std::vector<entt::entity> Children;
	public:
		unsigned int loadingParentID;
		TreeInfo() = default;
		const std::vector<entt::entity>& getChildren() const
		{
			return Children;
		}
		entt::entity getParent() const
		{
			return parent;
		}
		void setParent(entt::entity you, entt::entity other, entt::registry* registry)
		{
			if (!registry->any_of<TreeInfo>(other)) {
				std::cout << "no treeinfo on parent\n";
				return;
			}
			auto& parenttree = registry->get<TreeInfo>(other);
			if (registry->valid(parent))
			{
				registry->get<TreeInfo>(parent).removeChild(you, registry);
			}
			parent = other;

			if (registry->valid(parent) && registry->any_of<TreeInfo>(parent))
			{
				auto& newParentTree = registry->get<TreeInfo>(other);
				if (std::find(newParentTree.Children.begin(), newParentTree.Children.end(), you) == newParentTree.Children.end())
					newParentTree.Children.push_back(you);
			}
		}
		void addChild(entt::entity self, entt::entity child,entt::registry* registry)
		{
			if (registry->valid(child) && registry->any_of<TreeInfo>(child))
			{
				registry->get<TreeInfo>(child).setParent(child, self, registry);
				if (std::find(Children.begin(), Children.end(), child) == Children.end())
					Children.push_back(child);
			}
		}
		void removeChild(entt::entity other, entt::registry* registry)
		{
			auto& childtree = registry->get<TreeInfo>(other);
			auto it = std::find(Children.begin(), Children.end(), other);
			if (it == Children.end()) return;
			Children.erase(it);
			if (registry->valid(other) and registry->any_of<TreeInfo>(other))
			{
				
				childtree.parent = entt::null;
			}
			
			
		}
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(parent);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(parent);
		}
	};

}

namespace glm
{
	template <class Archieve>
	void save(Archieve& ar, const glm::vec4& v)
	{
		ar(v.x,v.y,v.z,v.w);

	}
	template <class Archieve>

	void load(Archieve& ar, glm::vec4& v)
	{
		ar(v.x, v.y, v.z, v.w);
	}
	template <class Archieve>
	void save(Archieve& ar, const glm::vec3& v)
	{
		ar(v.x, v.y, v.z);

	}
	template <class Archieve>

	void load(Archieve& ar, glm::vec3& v)
	{
		ar(v.x, v.y, v.z);
	}
}
namespace Cle
{
	//struct Dirty {};
	struct TextureRef
	{
		std::string path;
		bool operator==(const TextureRef& other) const
		{
			return path == other.path;
		}
	};

	struct MaterialRef
	{
		std::string name{};
		std::string colorMapPath{};
		glm::vec4 color;
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(name, colorMapPath, color);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(name, colorMapPath, color);
		}
	};
	struct MeshPacket
	{
		std::string path{};
		int meshIndex;
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(meshIndex, path);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(meshIndex, path);
		}
	};
	struct KeyPacket
	{
		unsigned char vk;
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(vk);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(vk);
		}
	};
	struct MovementPacket
	{
		bool vk[5] = { false };
		glm::vec3 forward;
		glm::vec3 right;
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(vk, forward, right);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(vk,forward,right);
		}
	};
	enum NetworkMessage : int
	{
		OnJoin,
		UpdateEntity,
		Input,
		MovementKey,
		UpdateCamera,
	};

	struct Header
	{
		int msg;
		template <class Archive>
		void save(Archive& ar) const
		{
			ar((int)msg);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(msg);
		}
	};

	struct networkID 
	{
		int value;
		networkID() = default;
		networkID(int v) : value(v) {}
		template <class Archieve>
		void save(Archieve& ar) const
		{
			ar(value);

		}
		template <class Archieve>

		void load(Archieve& ar)
		{
			ar(value);

		}
	};
	struct TreeInfoPacket
	{
		int parentNetID{};
		TreeInfoPacket() = default;
		TreeInfoPacket(int ID) : parentNetID(ID) {}
		template <class Archieve>
		void save(Archieve& ar) const
		{
			ar(parentNetID);

		}
		template <class Archieve>

		void load(Archieve& ar)
		{
			ar(parentNetID);

		}
	};
	struct EntityPacket
	{
		networkID netID;
		std::optional < Cle::Components::Color> color;
		std::optional <MeshPacket> mesh;
		std::optional <Cle::Components::Transform> transform;
		std::optional <TreeInfoPacket> treeinfo;
		template <class Archieve>
		void save(Archieve& ar) const
		{
			ar(netID,color,mesh,transform,treeinfo);

		}
		template <class Archieve>

		void load(Archieve& ar)
		{
			ar(netID, color, mesh, transform, treeinfo);

		}
	};
}
namespace std {
	template <>
	struct hash<Cle::TextureRef> {
		std::size_t operator()(const Cle::TextureRef& t) const noexcept {
			return std::hash<std::string>{}(t.path);
		}
	};
}

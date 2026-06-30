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
#include "Components/Transform.h"

namespace Cle::Components
{
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
			if (!registry->any_of<TreeInfo>(other)) return;
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
			childtree.parent = entt::null;
			
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
			ar(name, colorMapPath,color);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(name, colorMapPath,color);
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
	struct networkID 
	{
		int value;

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
	class EntityPacket
	{
	public:
		networkID m_networkID;
		std::optional<Cle::Components::Transform> transform;
		//std::optional<MeshPacket> mesh;
		std::optional<Cle::Components::Name> name;
		//std::optional<MaterialPacket> material;

		template <class Archieve>
		void save(Archieve& ar) const
		{
			ar(m_networkID, transform);

		}
		template <class Archieve>

		void load(Archieve& ar)
		{
			ar(m_networkID, transform);

		}
	};
}
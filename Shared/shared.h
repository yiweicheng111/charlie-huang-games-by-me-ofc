#pragma once
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>

#include <cereal/archives/binary.hpp>

#include "enet/enet.h"
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

	class Name
	{
	private:
		std::string value{};
		template <class Archive>
		friend void load(Archive& ar);
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
	struct TreeInfo
	{
		entt::entity parent = entt::null;
		std::vector<entt::entity> Children;
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

	class MaterialPacket
	{
	private:
		glm::vec3 color;
		std::string colorMap;
	public:
		bool dirty = false;
		MaterialPacket() = default;
		glm::vec3 getColor() const
		{
			return color;
		}
		std::string getColorMap() const
		{
			return colorMap;
		}
		void setColor(glm::vec3 othercolor)
		{
			color = othercolor;
			dirty = true;
		}
		void setColorMap(std::string othercolormap)
		{
			colorMap = othercolormap;
			dirty = true;
		}
		template <class Archieve>
		void save(Archieve& ar) const
		{
			ar(color,colorMap);
		}
		template <class Archieve>

		void load(Archieve& ar)
		{
			ar(color, colorMap);
			dirty = true;
		}
	};
		class MeshPacket
		{
		private:
			int meshIndex{};
			std::string path{};
			std::string texturePath{};
		public:

			bool dirty = false;

			int getMeshIndex() const
			{
				return meshIndex;
			}
			std::string getPath() const
			{
				return path;
			}
			std::string getTexturePath() const
			{
				return texturePath;
			}

			void setMeshIndex(int i) 
			{
				meshIndex = i;
				dirty = true;
			}
			void setPath(std::string _path) 
			{
				path = _path;
				dirty = true;
			}
			void setTexturePath(std::string path) 
			{
				texturePath = path;
				dirty = true;
			}
			MeshPacket(const std::string& path, int meshIndex) :path(path), meshIndex(meshIndex) {}
			MeshPacket() = default;
			static std::vector<MeshPacket> loadModel(const std::string& path)
			{
				std::vector<MeshPacket> total;
				Assimp::Importer importer;
				const aiScene* scene = importer.ReadFile(path, 0);
				for (int i = 0; i < scene->mNumMeshes; i++)
				{
					total.emplace_back(path, i);
				}
				return total;
			}
			template <class Archieve>
			void save(Archieve& ar) const
			{
				ar(path,texturePath,meshIndex);
			}
			template <class Archieve>

			void load(Archieve& ar)
			{
				ar(path, texturePath, meshIndex);
				dirty = true;
			}
		};

	
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
		std::optional<MeshPacket> mesh;
		std::optional<Cle::Components::Name> name;
		std::optional<MaterialPacket> material;

		template <class Archieve>
		void save(Archieve& ar) const
		{
			ar(m_networkID, transform, mesh);

		}
		template <class Archieve>

		void load(Archieve& ar)
		{
			ar(m_networkID, transform, mesh);

		}
	};
}
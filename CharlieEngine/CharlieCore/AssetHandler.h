#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Mesh.h"
#include "GenericMesh.h"
using namespace Cle::Gfx;
namespace Cle
{
	class AssetHandler {
	private:
		AssetHandler() = default;
	public:
		static AssetHandler& getInstance()
		{
			static AssetHandler AssetHandler{};
			return AssetHandler;
		}
		struct pairhash {
		public:
			template <typename T, typename U>
			std::size_t operator()(const std::pair<T, U>& x) const
			{
				return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
			}
		};
		std::unordered_map<std::pair<std::string, int>, std::shared_ptr<GenericMesh>, pairhash> meshCache;
		std::unordered_map<std::string, std::vector< std::shared_ptr<GenericMesh>>> modelCache;



		std::shared_ptr<GenericMesh> getOrMakeMesh(const std::string& path, int meshIndex)
		{
			if (meshCache.contains({ path,meshIndex }))
			{
				return meshCache[{ path, meshIndex }];
			}
			return meshCache[{ path, meshIndex }];
		}

		std::vector<GenericMesh> ProcessNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform);
		GenericMesh ProcessMesh(aiMesh* Mesh, const aiScene* scene, const glm::mat4& global);
		std::vector<std::shared_ptr<GenericMesh>>& LoadModel(std::string path);
	};
}
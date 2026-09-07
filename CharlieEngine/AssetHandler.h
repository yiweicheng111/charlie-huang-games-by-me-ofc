#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Mesh.h"
#include "GenericMesh.h"
using namespace Cle::Gfx;
namespace Cle
{
	struct CachedMeshInfo
	{
		std::vector<Cle::Gfx::Vertex> Vertices;
		std::vector<unsigned int> Indices;
		glm::vec3 scaleOffset = glm::vec3(1.0f);
		glm::vec3 positionOffset = glm::vec3(0.0f);
		glm::quat orientationOffset;
		glm::vec3 assimpRequestedColor = glm::vec3(-1, -1, -1);
		std::string assimpRequestedDiffuse = "";
	};
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
		std::unordered_map<std::pair<std::string,int>, std::shared_ptr<MeshGeometry>,pairhash> meshCache;
		std::unordered_map<std::string, std::vector< std::shared_ptr<GenericMesh>>> modelCache;

		bool UnloadModel(const std::string& path, int index, std::shared_ptr<GenericMesh>& gmeshp)
		{
			//std::cout << path << index <<std::endl;

			auto it = meshCache.find({ path, index });
			if (it == meshCache.end())

			{
			//	std::cout << " note here" << std::endl;

				return false;
			}

			auto& meshPtr = meshCache[{path, index}];
			std::cout << "ptr used " << meshPtr.use_count() << std::endl;
			auto& vec = modelCache[path];
		   // vec.erase(std::remove(vec.begin(), vec.end(), gmeshp), vec.end());
			if (meshPtr.use_count() > 3){
				std::cout << "still some instances cant delete yet\n";
				return false;  
			}
			std::cout << "erased\n";
			meshCache.erase(it);
			return true;

		}

		/*std::shared_ptr<GenericMesh> getOrMakeMesh(const std::string& path, int meshIndex)
		{
			const auto& meshes = LoadModel(path);
			if (meshes.empty())
				return nullptr;

			if (meshIndex < 0 ||
				meshIndex >= static_cast<int>(meshes.size()))
			{
				meshIndex = static_cast<int>(meshes.size()) - 1;
			}
	
			return  modelCache[path].at(meshIndex);
		}*/

		std::vector<GenericMesh> ProcessNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform,  std::unordered_map<unsigned int, std::shared_ptr<MeshGeometry>>& tempcache);
		std::shared_ptr<MeshGeometry> ProcessMesh(aiMesh* Mesh, const aiScene* scene);
		std::vector<std::shared_ptr<GenericMesh>>& LoadModel(std::string path);
	};
}
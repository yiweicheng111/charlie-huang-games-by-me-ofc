#pragma once
#include <vector>
#include "GfxBase.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Components/Components.h"
#include <string>
#include "ITexture.h"
namespace Cle::Gfx
{

	class GenericMesh {
	private:
		std::vector<Cle::Gfx::Vertex> Vertices;
		std::vector<unsigned int> Indices;
	public:
		int loadedMeshIndex = 0;

		GenericMesh() {};
		GenericMesh(const GenericMesh& other) = default;

		glm::vec3 offset{};
		float scaleOffset = 1;

		std::string ModelPath = "";
		
		Cle::Components::AABB m_local_AABB;
		Cle::Components::AABB m_AABB;
		Cle::Components::Sphere m_local_Bounding_Sphere;
		Cle::Components::Sphere m_Bounding_Sphere;

	    std::shared_ptr<Cle::Gfx::ITexture> texture;

		glm::vec3 assimpRequestedColor = glm::vec3(-1, -1, -1);
		std::string assimpRequestedDiffuse = "";

		bool verticesDirty = false;
		bool indicesDirty = false;


		void setVertices(std::vector<Cle::Gfx::Vertex>& vother) {
			indicesDirty = true;
			Vertices = vother;
		}

		void setIndices(std::vector<unsigned int>& indother) {
			indicesDirty = true;
			Indices = indother;
		}

		void moveVertex(int idx, glm::vec3 pos) {
			indicesDirty = true;
		}

		const std::vector<Cle::Gfx::Vertex>& getVertices() const  {
			return Vertices;
		}
		const std::vector<unsigned int>& getIndices() const {
			return Indices;
		}
		GenericMesh(const std::vector<Cle::Gfx::Vertex>& _Vertices) : Vertices(_Vertices) {
			if (_Vertices.empty()) return;
			m_AABB = Cle::Components::AABB(_Vertices);
			m_local_AABB = Cle::Components::AABB(_Vertices);
		}	
		GenericMesh(const std::vector<Cle::Gfx::Vertex>& _Vertices, const std::vector<unsigned int>& _Indices) : Vertices(_Vertices), Indices(_Indices) {
			if (_Vertices.empty()) return;
			m_AABB=Cle::Components::AABB(_Vertices);
			m_local_AABB = Cle::Components::AABB(_Vertices);
			m_local_Bounding_Sphere = Cle::Components::Sphere(_Vertices);
			m_Bounding_Sphere = Cle::Components::Sphere(_Vertices);

		}
	};
	struct IMesh {
		std::unordered_map<unsigned int, std::vector<unsigned int>> LODIndicesEBOMap;


		virtual void draw() = 0;
	};
	struct AssetManager {
		std::unordered_map<std::string, std::shared_ptr<ITexture>> textureCache;
		std::vector<GenericMesh> ProcessNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform);
		GenericMesh ProcessMesh(aiMesh* Mesh, const aiScene* scene, const glm::mat4& global);
		std::vector<GenericMesh> LoadModel(std::string path);
	};
}
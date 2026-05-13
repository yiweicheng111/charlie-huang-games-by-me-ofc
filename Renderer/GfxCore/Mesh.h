#pragma once
#include <vector>
#include "GfxBase.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Components/Components.h"
namespace Cle::Gfx
{
	struct IMesh {
		virtual void draw() = 0;
	};
	class GenericMesh {
	private:
		std::vector<Cle::Gfx::Vertex> Vertices;
		std::vector<unsigned int> Indices;
	public:
		enum DirtyLevel {
			VERTEX,
			VERTICES,
			NONE
		};
		GenericMesh() {};
		GenericMesh(const GenericMesh& other) = default;

		glm::vec3 offset{};
		bool OffsetDirty = false;
		GenericMesh::DirtyLevel Dirty = DirtyLevel::NONE;
		Cle::Components::AABB m_local_AABB;
		Cle::Components::AABB m_AABB;
		Cle::Components::Sphere m_local_Bounding_Sphere;
		Cle::Components::Sphere m_Bounding_Sphere;
		std::string assimpRequestedTexturePath = "";
		glm::vec3 assimpRequestedColor = glm::vec3(-1,-1,-1);
		void setVertices(std::vector<Cle::Gfx::Vertex>& vother) {
			Dirty = DirtyLevel::VERTICES;
			Vertices = vother;
		}

		void setIndices(std::vector<unsigned int>& indother) {
			Dirty = DirtyLevel::VERTICES;
			Indices = indother;
		}

		void moveVertex(int idx, glm::vec3 pos) {
			Dirty = DirtyLevel::VERTEX;
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
	struct GenericMeshHandler {
		std::vector<GenericMesh> ProcessNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform);
		GenericMesh ProcessMesh(aiMesh* Mesh, const aiScene* scene);
		std::vector<GenericMesh> LoadModel(const char* path);
	};
}
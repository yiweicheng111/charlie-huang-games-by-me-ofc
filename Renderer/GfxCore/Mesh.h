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
	struct GenericMesh {
		GenericMesh() {};
		Cle::Components::AABB m_AABB;
		std::vector<Cle::Gfx::Vertex> Vertices;
		std::vector<unsigned int> Indices;
		GenericMesh(std::vector<Cle::Gfx::Vertex>& _Vertices) : Vertices(_Vertices) {}
		GenericMesh(std::vector<Cle::Gfx::Vertex>& _Vertices, std::vector<unsigned int>& _Indices) : Vertices(_Vertices), Indices(_Indices) {
			m_AABB=Cle::Components::AABB(_Vertices);
		}
	};
	struct GenericMeshHandler {
		std::vector<GenericMesh> ProcessNode(aiNode* node, const aiScene* scene);
		GenericMesh ProcessMesh(aiMesh* Mesh, const aiScene* scene);
		std::vector<GenericMesh> LoadModel(const char* path);
	};
}
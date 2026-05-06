#include "Mesh.h"
#include <iostream>
#include <filesystem>
#include "OpenGL4/OpenGLMesh.h"
using namespace Cle::Gfx;
std::vector<GenericMesh> GenericMeshHandler::ProcessNode(aiNode* node, const aiScene* scene)
{
	std::cout << "processed\n";

	std::vector<GenericMesh> meshes;
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		auto children = ProcessNode(node->mChildren[i], scene);
		meshes.insert(meshes.end(), children.begin(), children.end());

	}
	return meshes;
}
GenericMesh GenericMeshHandler::ProcessMesh(aiMesh* Mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < Mesh->mNumVertices; i++) {
		Vertex v{};
		v.Position = glm::vec3(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z);
		if (Mesh->mNormals) v.Normal = glm::vec3(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z);

		if (Mesh->mTextureCoords[0]) {
			v.texCoord = glm::vec2(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y);
		}
		else {
			v.texCoord = glm::vec2(0, 0);
		}
		vertices.push_back(v);
	}
	for (unsigned int i = 0; i < Mesh->mNumFaces; i++) {
		aiFace face = Mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	return Cle::Gfx::GenericMesh(vertices, indices);
}
std::vector<GenericMesh> GenericMeshHandler::LoadModel(const char* path)
{
	
	if (!std::filesystem::exists(path)) {
		std::cerr << path << " does not exist\n"; return{};
	};
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_Triangulate | aiProcess_FlipUVs
	);

	return ProcessNode(scene->mRootNode, scene);
}

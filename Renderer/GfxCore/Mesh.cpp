#include "Mesh.h"
#include <iostream>
#include <filesystem>
#include "OpenGL4/OpenGLMesh.h"
using namespace Cle::Gfx;
static glm::mat4 toGlm(aiMatrix4x4 mat)
{
	return glm::mat4(
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4
	);
}
std::vector<GenericMesh> GenericMeshHandler::ProcessNode(aiNode* node, const aiScene* scene, aiMatrix4x4 pTransform)
{

	std::vector<GenericMesh> meshes;
	aiMatrix4x4 aiGlobal = pTransform * node->mTransformation;
	glm::mat4 global = toGlm(pTransform * node->mTransformation);
	meshes.reserve(node->mNumMeshes);
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		GenericMesh gMesh = ProcessMesh(mesh, scene);
		glm::vec4 world = global * glm::vec4(gMesh.offset,1.0f);
		gMesh.offset = glm::vec3(world);
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		auto children = ProcessNode(node->mChildren[i], scene, aiGlobal);
		meshes.insert(meshes.end(), children.begin(), children.end());
	}
	return meshes;
}
GenericMesh GenericMeshHandler::ProcessMesh(aiMesh* Mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	vertices.reserve(Mesh->mNumVertices);
	std::vector<unsigned int> indices;
	indices.reserve(Mesh->mNumFaces*3);
	glm::vec3 max = glm::vec3(Mesh->mVertices[0].x, Mesh->mVertices[0].y, Mesh->mVertices[0].z);
	glm::vec3 min = glm::vec3(Mesh->mVertices[0].x, Mesh->mVertices[0].y, Mesh->mVertices[0].z);
	for (unsigned int i = 0; i < Mesh->mNumVertices; i++) {
		Vertex v{};
		v.Position = glm::vec3(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z);

		max = glm::max(v.Position, max);
		min = glm::min(v.Position, min);

		if (Mesh->mNormals) v.Normal = glm::vec3(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z);

		if (Mesh->mTextureCoords[0]) {
			v.texCoord = glm::vec2(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y);
		}
		else {
			v.texCoord = glm::vec2(0, 0);
		}
		vertices.push_back(v);
	}
	glm::vec3 center = (max + min) / 2.0f;
	for (auto& i : vertices) {
		i.Position -= center;
	}
	for (unsigned int i = 0; i < Mesh->mNumFaces; i++) {
		aiFace face = Mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	Cle::Gfx::GenericMesh loadedMesh(vertices, indices);
	loadedMesh.offset = center;
	if (Mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[Mesh->mMaterialIndex];
		aiString path;
		aiColor3D color(1.0f, 1.0f, 1.0f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		loadedMesh.assimpRequestedColor = glm::vec3(color.r, color.g, color.b);

		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path))
		{
			loadedMesh.assimpRequestedTexturePath = path.C_Str();
		}

	}
	return loadedMesh;
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

	return ProcessNode(scene->mRootNode, scene, aiMatrix4x4(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	));
}

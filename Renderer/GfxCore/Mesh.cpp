#include "Mesh.h"
#include <iostream>
#include <filesystem>
#include "OpenGL4/OpenGLMesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace Cle::Gfx;
static glm::mat4 toGlm(aiMatrix4x4 mat)
{
	return glm::transpose(glm::make_mat4(&mat.a1));
}
std::vector<GenericMesh>AssetManager::ProcessNode(aiNode* node, const aiScene* scene, aiMatrix4x4 pTransform)
{

	std::vector<GenericMesh> meshes;
	aiMatrix4x4 aiGlobal = pTransform * node->mTransformation;
	glm::mat4 global = toGlm(pTransform * node->mTransformation);
	meshes.reserve(node->mNumMeshes);
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		GenericMesh gMesh = ProcessMesh(mesh, scene,global);
		meshes.push_back(gMesh);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		auto children = ProcessNode(node->mChildren[i], scene, aiGlobal);
		meshes.insert(meshes.end(), children.begin(), children.end());
	}
	return meshes;
}
GenericMesh AssetManager::ProcessMesh(aiMesh* Mesh, const aiScene* scene, const glm::mat4& global)
{
	std::vector<Vertex> vertices;
	vertices.reserve(Mesh->mNumVertices);
	std::vector<unsigned int> indices;
	indices.reserve(Mesh->mNumFaces*3);

	glm::vec3 max = glm::vec3(global * glm::vec4(Mesh->mVertices[0].x, Mesh->mVertices[0].y, Mesh->mVertices[0].z,1.0f));
	glm::vec3 min = max;
	for (unsigned int i = 0; i < Mesh->mNumVertices; i++) {
		Vertex v{};

		v.Position = glm::vec3(global*glm::vec4(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z,1));

		max = glm::max(v.Position, max);
		min = glm::min(v.Position, min);

		if (Mesh->mNormals)
		{
			v.Normal = glm::normalize(glm::vec3(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z)*glm::mat3(glm::transpose(glm::inverse(global))));

		}

		if (Mesh->mTextureCoords[0]) {
			v.texCoord = glm::vec2(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y);

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

		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			loadedMesh.assimpRequestedDiffuse = std::string("map/")+path.C_Str();
		}

	}
	return loadedMesh;
}
std::vector<GenericMesh>& AssetManager::LoadModel(std::string path)
{
	if (!std::filesystem::exists(path)) {
		meshCache[path] = {};
		std::cerr << path << " does not exist\n"; //return meshCache[path];
		return meshCache[path];
	};
	if (meshCache.contains(path))
	{
		return meshCache[path];
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenUVCoords | aiProcess_GenUVCoords 
	);

	std::vector<GenericMesh> Meshes = ProcessNode(scene->mRootNode, scene, aiMatrix4x4());
	int i = 0;
	for (auto& Mesh : Meshes)
	{
		Mesh.ModelPath = path;
		Mesh.loadedMeshIndex = i;
		i++;
	}
	meshCache[path] = Meshes;
	return meshCache[path];
}

#include "Mesh.h"
#include "AssetHandler.h"
#include <iostream>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GenericMesh.h"
using namespace Cle::Gfx;
using namespace Cle;
static glm::mat4 toGlm(aiMatrix4x4 mat)
{
	return glm::transpose(glm::make_mat4(&mat.a1));
}
std::vector<GenericMesh>AssetHandler::ProcessNode(aiNode* node, const aiScene* scene, aiMatrix4x4 pTransform)
{

	std::vector<GenericMesh> meshes;
	aiMatrix4x4 aiGlobal = pTransform * node->mTransformation;
	aiQuaternion rot;
	aiVector3D scale;
	aiVector3D pos;
	aiGlobal.Decompose(scale, rot ,pos);
	meshes.reserve(node->mNumMeshes);
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		GenericMesh gMesh = ProcessMesh(mesh, scene);
		gMesh.scaleOffset = glm::vec3(scale.x, scale.y, scale.z);
		gMesh.orientationOffset = glm::quat(rot.w,rot.x, rot.y, rot.z);
		gMesh.positionOffset = glm::vec3(pos.x, pos.y, pos.z);

		meshes.push_back(gMesh);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		auto children = ProcessNode(node->mChildren[i], scene, aiGlobal);
		meshes.insert(meshes.end(), children.begin(), children.end());
	}
	return meshes;
}

GenericMesh AssetHandler::ProcessMesh(aiMesh* Mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	vertices.reserve(Mesh->mNumVertices);
	std::vector<unsigned int> indices;
	indices.reserve(Mesh->mNumFaces * 3);

	glm::vec3 max = glm::vec3( glm::vec4(Mesh->mVertices[0].x, Mesh->mVertices[0].y, Mesh->mVertices[0].z, 1.0f));
	glm::vec3 min = max;
	for (unsigned int i = 0; i < Mesh->mNumVertices; i++) {
		Vertex v{};

		v.Position = glm::vec3(glm::vec4(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z, 1));

		max = glm::max(v.Position, max);
		min = glm::min(v.Position, min);

		if (Mesh->mNormals)
		{
			v.Normal = glm::normalize(glm::vec3(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z));

		}

		if (Mesh->mTextureCoords[0]) {
			v.texCoord = glm::vec2(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y);

		}


		vertices.push_back(v);
	}
	glm::vec3 center = (max + min) / 2.0f;
	float maxV = std::max({ max.x,max.y,max.z });

	for (unsigned int i = 0; i < Mesh->mNumFaces; i++) {
		aiFace face = Mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	GenericMesh loadedMesh(vertices, indices);
	if (Mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[Mesh->mMaterialIndex];
		aiString path;
		aiColor3D color(1.0f, 1.0f, 1.0f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		loadedMesh.assimpRequestedColor = glm::vec3(color.r, color.g, color.b);

		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			loadedMesh.assimpRequestedDiffuse = std::string("map/") + path.C_Str();
		}

	}
	return loadedMesh;
}

std::vector<std::shared_ptr<GenericMesh>>& AssetHandler::LoadModel(std::string path)
{
	if (!std::filesystem::exists(path)) {
		modelCache[path] = {};
		std::cerr << path << " does not exist\n"; //return meshCache[path];
		return modelCache[path];
	};
	if (modelCache.contains(path))
	{
		return modelCache[path];
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenUVCoords | aiProcess_GenUVCoords | aiProcess_JoinIdenticalVertices
	);

	std::vector<GenericMesh> Meshes = ProcessNode(scene->mRootNode, scene, aiMatrix4x4());
	auto& entry = modelCache[path];
	entry.reserve(Meshes.size());
	int i = 0;
	for (auto& Mesh : Meshes)
	{
		Mesh.setModelPath(path);
		Mesh.setMeshIndex(i);
		auto ptr = std::make_shared<GenericMesh>(std::move(Mesh));
		meshCache[{path, i}] = ptr;
		entry.push_back(std::move(ptr));
		i++;
	}
	return entry;
}

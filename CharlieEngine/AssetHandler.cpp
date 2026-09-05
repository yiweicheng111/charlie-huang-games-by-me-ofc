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



std::vector<GenericMesh> AssetHandler::ProcessNode(
    aiNode* node,
    const aiScene* scene,
    aiMatrix4x4 pTransform,
    std::unordered_map<
    unsigned int,
    std::shared_ptr<MeshGeometry>
    >& tempcache)
{
    std::vector<GenericMesh> meshes;

    aiMatrix4x4 aiGlobal =
        pTransform * node->mTransformation;

    aiQuaternion rot;
    aiVector3D scale;
    aiVector3D pos;

    aiGlobal.Decompose(
        scale,
        rot,
        pos
    );

    meshes.reserve(node->mNumMeshes);


 
    for (unsigned int i = 0;
        i < node->mNumMeshes;
        i++)
    {
        unsigned int index = node->mMeshes[i];


        auto it = tempcache.find(index);


        if (it == tempcache.end())
        {
            auto geometry =
                ProcessMesh(
                    scene->mMeshes[index],
                    scene
                );

            it = tempcache.emplace(
                index,
                geometry
            ).first;
        }



        GenericMesh gMesh(it->second);

        gMesh.scaleOffset =
            glm::vec3(
                scale.x,
                scale.y,
                scale.z
            );

        gMesh.orientationOffset =
            glm::quat(
                rot.w,
                rot.x,
                rot.y,
                rot.z
            );

        gMesh.positionOffset =
            glm::vec3(
                pos.x,
                pos.y,
                pos.z
            );
        gMesh.geometry->instancedIndex = node->mMeshes[i];
        auto Mesh = scene->mMeshes[node->mMeshes[i]];
        if (Mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[Mesh->mMaterialIndex];
            aiString path;
            aiColor3D color(1.0f, 1.0f, 1.0f);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            gMesh.assimpRequestedColor = glm::vec3(color.r, color.g, color.b);
         //   std::cout << color.r << " " << color.g << " " << color.b << std::endl;

            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            {
                gMesh.assimpRequestedDiffuse = std::string("map/") + path.C_Str();
            }

        }

        meshes.push_back(
            std::move(gMesh)
        );
    }

    for (unsigned int i = 0;
        i < node->mNumChildren;
        i++)
    {
        auto children =
            ProcessNode(
                node->mChildren[i],
                scene,
                aiGlobal,
                tempcache
            );

        meshes.insert(
            meshes.end(),
            children.begin(),
            children.end()
        );
    }


    return meshes;
}



std::shared_ptr<MeshGeometry> AssetHandler::ProcessMesh(
    aiMesh* Mesh,
    const aiScene* scene)
{
    std::vector<Vertex> vertices;

    vertices.reserve(
        Mesh->mNumVertices
    );


    std::vector<unsigned int> indices;

    indices.reserve(
        Mesh->mNumFaces * 3
    );



    glm::vec3 max = glm::vec3(
        glm::vec4(
            Mesh->mVertices[0].x,
            Mesh->mVertices[0].y,
            Mesh->mVertices[0].z,
            1.0f
        )
    );

    glm::vec3 min = max;


    for (unsigned int i = 0;
        i < Mesh->mNumVertices;
        i++)
    {
        Vertex v{};


        v.Position =
            glm::vec3(
                glm::vec4(
                    Mesh->mVertices[i].x,
                    Mesh->mVertices[i].y,
                    Mesh->mVertices[i].z,
                    1.0f
                )
            );


        max = glm::max(
            v.Position,
            max
        );

        min = glm::min(
            v.Position,
            min
        );


        if (Mesh->mNormals)
        {
            v.Normal =
                glm::normalize(
                    glm::vec3(
                        Mesh->mNormals[i].x,
                        Mesh->mNormals[i].y,
                        Mesh->mNormals[i].z
                    )
                );
        }


        if (Mesh->mTextureCoords[0])
        {
            v.texCoord =
                glm::vec2(
                    Mesh->mTextureCoords[0][i].x,
                    Mesh->mTextureCoords[0][i].y
                );
        }


        vertices.push_back(v);
    }




    for (unsigned int i = 0;
        i < Mesh->mNumFaces;
        i++)
    {
        aiFace face =
            Mesh->mFaces[i];

        for (unsigned int j = 0;
            j < face.mNumIndices;
            j++)
        {
            indices.push_back(
                face.mIndices[j]
            );
        }
    }


    auto geometry =
        std::make_shared<MeshGeometry>();

    geometry->Vertices =
        std::move(vertices);

    geometry->Indices =
        std::move(indices);



    geometry->m_local_AABB =
        Cle::Components::AABB(geometry->Vertices);

    geometry->m_local_Bounding_Sphere =
        Cle::Components::Sphere(geometry->Vertices);


    return geometry;
}



std::vector<std::shared_ptr<GenericMesh>>&
AssetHandler::LoadModel(std::string path)
{

    if (!std::filesystem::exists(path))
    {
        modelCache[path] = {};

        std::cerr
            << path
            << " does not exist\n";

        return modelCache[path];
    }


    if (modelCache.contains(path))
    {
      //  std::cout << "number of unqiue models" << modelCache.size()<<std::endl;
        return modelCache[path];
    }

    auto start = time(NULL);
    Assimp::Importer importer;

    const aiScene* scene =
        importer.ReadFile(
            path,

            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_GenUVCoords |
            aiProcess_JoinIdenticalVertices |
            aiProcess_FindInstances
        );


    if (!scene || !scene->mRootNode)
    {
        std::cerr
            << "Failed to load model: "
            << path
            << '\n';

        modelCache[path] = {};

        return modelCache[path];
    }


    std::unordered_map<
        unsigned int,
        std::shared_ptr<MeshGeometry>
    > tempcache;

    std::vector<GenericMesh> meshes =
        ProcessNode(
            scene->mRootNode,
            scene,
            aiMatrix4x4(),
            tempcache
        );


/*
*  std::cout
        << "Assimp mesh count: "
        << scene->mNumMeshes
        << '\n';

    std::cout
        << "Instance count: "
        << meshes.size()
        << '\n';

    std::cout
        << "Unique geometry count: "
        << tempcache.size()
        << '\n';
*/
   

    auto& entry =
        modelCache[path];

    entry.reserve(
        meshes.size()
    );


    int instanceIndex = 0;


    for (auto& mesh : meshes)
    {
        mesh.setModelPath(path);

        mesh.setMeshIndex(instanceIndex);

        auto ptr =
            std::make_shared<GenericMesh>(
                std::move(mesh)
            );


        entry.push_back(ptr);
        instanceIndex++;
    }


    for (auto& [assimpMeshIndex, geometry] : tempcache)
    {
      //  std::cout << assimpMeshIndex << "index" << std::endl;
        meshCache[
        {
            path,
                static_cast<int>(assimpMeshIndex)
        }
        ] = geometry;
    }
    std::cout << "time used loading " << time(NULL) - start << " seconds meshes " << scene->mNumMeshes << std::endl;


    return entry;
}

#pragma once
#include "CharlieEngine/Components.h"
#include "Mesh.h"
namespace Cle
{
    namespace Gfx
    {
        struct IMesh;
    }
   
    struct MeshGeometry
    {
        std::vector<Cle::Gfx::Vertex> Vertices;
        std::vector<unsigned int> Indices;
        Cle::Components::AABB m_local_AABB;
        Cle::Components::Sphere m_local_Bounding_Sphere;
        std::shared_ptr<Cle::Gfx::IMesh> gpuMesh;
        bool gpuUploaded = false;
        int instancedIndex = 0;
        MeshGeometry()
        {
          //  std::cout << "MeshGeometry constructed at " << this << "\n";
        }
    };
    class GenericMesh {
    private:
        std::string ModelPath = "";
        int loadedMeshIndex = 0;

    public:
        std::shared_ptr< MeshGeometry> geometry = std::make_shared< MeshGeometry>();

        ~GenericMesh()
        {

        }
        GenericMesh(std::shared_ptr< MeshGeometry> geo)
        {
            geometry = geo;
            
        }
        bool modelPathDirty = false;
        bool modelIndexDirty = false;
        const void* getGeoID() const {
            return geometry.get();
        }
        void setModelPath(const std::string& path)
        {
            ModelPath = path;
            modelPathDirty = true;

        }
        void setMeshIndex(int index)
        {
            loadedMeshIndex = index;
            modelIndexDirty = true;
        }
        const std::string& getModelPath() const
        {
            return ModelPath;
        }
        int getMeshIndex() const
        {
            return loadedMeshIndex;
        }
        GenericMesh() {};
        GenericMesh(std::string mPath, int index) : ModelPath(mPath), loadedMeshIndex(index) {}
        GenericMesh(const GenericMesh& other) = default;

        glm::vec3 offset{};
        glm::vec3 scaleOffset = glm::vec3(1.0f);
        glm::vec3 positionOffset = glm::vec3(0.0f);
        glm::quat orientationOffset;

 


        glm::vec3 assimpRequestedColor = glm::vec3(-1, -1, -1);
        std::string assimpRequestedDiffuse = "";

        bool verticesDirty = false;
        bool indicesDirty = false;


        void setVertices(const std::vector<Cle::Gfx::Vertex>& vother) {
            indicesDirty = true;
            if (geometry.use_count() > 1)
            {
                geometry = std::make_shared<MeshGeometry>(*geometry);
            }
            geometry->Vertices = vother;
        }

        void setIndices(const std::vector<unsigned int>& indother) {
            indicesDirty = true;
            if (geometry.use_count() > 1)
            {
                geometry = std::make_shared<MeshGeometry>(*geometry);
            }
            geometry->Indices = indother;
        }

        void moveVertex(int idx, glm::vec3 pos) {
            indicesDirty = true;
        }

        const std::vector<Cle::Gfx::Vertex>& getVertices() const {
            return geometry->Vertices;
        }
        const std::vector<unsigned int>& getIndices() const {
            return  geometry->Indices;
        }
        GenericMesh(const std::vector<Cle::Gfx::Vertex>& _Vertices) {

            if (_Vertices.empty()) return;
            setVertices(_Vertices);
            geometry->m_local_AABB = Cle::Components::AABB(_Vertices);
            geometry->m_local_Bounding_Sphere = Cle::Components::Sphere(_Vertices);
        }
        GenericMesh(const std::vector<Cle::Gfx::Vertex>& _Vertices, const std::vector<unsigned int>& _Indices) {

            if (_Vertices.empty()) return;
            setVertices(_Vertices);
            setIndices(_Indices);
            geometry->m_local_AABB = Cle::Components::AABB(_Vertices);
            geometry->m_local_Bounding_Sphere = Cle::Components::Sphere(_Vertices);

        }

        template <class Archive>
        void save(Archive& ar) const
        {
            ar(ModelPath, loadedMeshIndex);
        }
        template <class Archive>
        void load(Archive& ar)
        {
            ar(ModelPath, loadedMeshIndex);
        }
    };
}
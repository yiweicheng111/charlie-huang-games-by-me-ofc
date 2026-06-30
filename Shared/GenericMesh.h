#pragma once
#include "Components/Components.h"
namespace Cle
{

	class GenericMesh {
	private:
		std::vector<Cle::Gfx::Vertex> Vertices;
		std::vector<unsigned int> Indices;
		std::string ModelPath = "";
		int loadedMeshIndex = 0;

	public:
		bool modelPathDirty = false;
		bool modelIndexDirty = false;
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
		bool gpuUploaded = false;
		GenericMesh() {};
		GenericMesh(const GenericMesh& other) = default;

		glm::vec3 offset{};
		float scaleOffset = 1;


		Cle::Components::AABB m_local_AABB;
		Cle::Components::Sphere m_local_Bounding_Sphere;


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

		const std::vector<Cle::Gfx::Vertex>& getVertices() const {
			return Vertices;
		}
		const std::vector<unsigned int>& getIndices() const {
			return Indices;
		}
		GenericMesh(const std::vector<Cle::Gfx::Vertex>& _Vertices) : Vertices(_Vertices) {
			if (_Vertices.empty()) return;
			m_local_AABB = Cle::Components::AABB(_Vertices);
			m_local_Bounding_Sphere = Cle::Components::Sphere(_Vertices);
		}
		GenericMesh(const std::vector<Cle::Gfx::Vertex>& _Vertices, const std::vector<unsigned int>& _Indices) : Vertices(_Vertices), Indices(_Indices) {
			if (_Vertices.empty()) return;
			m_local_AABB = Cle::Components::AABB(_Vertices);
			m_local_Bounding_Sphere = Cle::Components::Sphere(_Vertices);

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
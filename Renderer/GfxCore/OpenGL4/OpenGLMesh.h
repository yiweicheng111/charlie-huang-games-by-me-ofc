#pragma once
#include "Mesh.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "VAO.h"
#include "EBO.h"
#include "VBO.h"
#include "GfxBase.h"
#include "meshoptimizer/src/meshoptimizer.h"
#include <iostream>
#include <cereal/types/polymorphic.hpp>
#include "GenericMesh.h"
namespace Cle::OPENGL 
{
	
	class Mesh : public Cle::Gfx::IMesh
	{
	private:
	public:
		std::shared_ptr<Cle::OPENGL::Mesh> getLodMesh(int index) 
		{
			if (index >= LODmeshes.size()) return nullptr;
			return std::static_pointer_cast<Cle::OPENGL::Mesh>(LODmeshes.at(index));
		}
		const std::vector<std::shared_ptr<Cle::Gfx::IMesh>>& getLodMesh() const
		{
			return LODmeshes;
		}
		void addLodMesh(std::shared_ptr<Cle::Gfx::IMesh> mesh)
		{
			LODmeshes.push_back(mesh);
		}
		unsigned int m_VAO{}, m_VBO{}, m_EBO{};
		

		virtual void draw() override;

		Mesh() = default;
		~Mesh() {
			glDeleteBuffers(1, &m_VBO);  glDeleteBuffers(1, &m_EBO); glDeleteVertexArrays(1, &m_VAO);
			
		}

		Mesh(unsigned int VAO, unsigned int VBO)
			: m_VBO(VBO), m_VAO(VAO) {
		}
		Mesh(unsigned int VAO, unsigned int VBO, unsigned int EBO)
			: m_VBO(VBO), m_VAO(VAO), m_EBO(EBO) {
		}
		Mesh(unsigned int VAO, unsigned int VBO, unsigned int EBO, unsigned int _indexCount)
			: m_VBO(VBO), m_VAO(VAO), m_EBO(EBO) {
			indexCount = _indexCount;
		}
		void draw(unsigned int nebo, unsigned int count);
		Mesh(std::shared_ptr<Cle::GenericMesh> g_Mesh)
		{
			auto& indices = g_Mesh->getIndices();
			auto& vertices = g_Mesh->getVertices();
			indexCount = indices.size();
			vertexCount = vertices.size();
			VBO vbo(vertices);
			VAO vao(vbo.ID);
			EBO ebo(indices);

			vao.LinkAttrib(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
			vao.LinkAttrib(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 3);
			vao.LinkAttrib(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 5);

			m_VAO = vao.ID;
			m_VBO = vbo.ID;
			m_EBO = ebo.ID;
			//std::cout << "size" << indices.size() << std::endl;
			if (indices.size() < 100) return;
			for (int i = 2; i < 5; i++)
			{
				size_t sindex = size_t(indices.size() * 1. / (i*i));
				float error = 110.5;
				std::vector<unsigned int> newIndices(indices.size());
				int nsize = meshopt_simplifySloppy(newIndices.data(), indices.data(), indices.size(), &vertices[0].Position.x, vertices.size(), sizeof(Cle::Gfx::Vertex), sindex, error);
				newIndices.resize(nsize);
				newIndices.shrink_to_fit();

				EBO nebo(newIndices);

				addLodMesh(std::make_shared<Cle::OPENGL::Mesh>(m_VAO, m_VBO, nebo.ID, newIndices.size()));
			}
		}



		
	};
	
}

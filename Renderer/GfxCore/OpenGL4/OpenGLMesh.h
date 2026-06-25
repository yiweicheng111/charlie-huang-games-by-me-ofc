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

namespace Cle::OPENGL 
{
	
	struct Mesh : Cle::Gfx::IMesh
	{

		int indexCount, vertexCount = 0;
		unsigned int m_VAO{}, m_VBO{}, m_EBO{};

		virtual void draw() override;

		Mesh() {}
		~Mesh() {
			glDeleteBuffers(1, &m_VBO);  glDeleteBuffers(1, &m_EBO); glDeleteVertexArrays(1, &m_VAO);
			for (auto& pair : LODIndicesEBOMap)
			{
				glDeleteBuffers(1, &pair.first);
			}
		}

		Mesh(unsigned int VAO, unsigned int VBO)
			: m_VBO(VBO), m_VAO(VAO) {
		}
		Mesh(unsigned int VAO, unsigned int VBO, unsigned int EBO)
			: m_VBO(VBO), m_VAO(VAO), m_EBO(EBO) {
		}
		Mesh(Cle::Gfx::GenericMesh g_Mesh)
		{
			gMesh = g_Mesh;
			auto& indices = g_Mesh.getIndices();
			auto& vertices = g_Mesh.getVertices();
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
			LODIndicesEBOMap[ebo.ID] = indices;
			//std::cout << "size" << indices.size() << std::endl;

			if (indices.size() < 150) return;
			for (int i = 2; i < 5; i++)
			{
				size_t sindex = size_t(indices.size() * 1. / (i*i*i));
				if (sindex < 6) sindex = 6;
				float error = 210.5;
				std::vector<unsigned int> newIndices(indices.size());
				int nsize = meshopt_simplifySloppy(newIndices.data(), indices.data(), indices.size(), &vertices[0].Position.x, vertices.size(), sizeof(Cle::Gfx::Vertex), sindex, error);
				newIndices.resize(nsize);
				newIndices.shrink_to_fit();

				EBO nebo(newIndices);

				LODIndicesEBOMap[nebo.ID] = newIndices;

			}
		}

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(cereal::base_class<Cle::Gfx::IMesh>(this));
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(cereal::base_class<Cle::Gfx::IMesh>(this));
		}

		
	};
	
}
CEREAL_REGISTER_TYPE(Cle::OPENGL::Mesh);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Cle::Gfx::IMesh, Cle::OPENGL::Mesh);

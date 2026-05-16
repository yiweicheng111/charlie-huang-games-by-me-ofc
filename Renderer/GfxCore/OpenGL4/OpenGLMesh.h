#pragma once
#include "Mesh.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "VAO.h"
#include "EBO.h"
#include "VBO.h"
#include "GfxBase.h"
#include <iostream>
namespace Cle::OPENGL 
{
	struct Mesh : Cle::Gfx::IMesh
	{
		
		int indexCount, vertexCount;
		unsigned int m_VAO{}, m_VBO{}, m_EBO{};
		Mesh() {}
		~Mesh() {
			glDeleteBuffers(1, &m_VBO);  glDeleteBuffers(1, &m_EBO); glDeleteVertexArrays(1, &m_VAO);
		}

		virtual void draw() override;
		Mesh(unsigned int VAO, unsigned int VBO)
			: m_VBO(VBO), m_VAO(VAO) {
		}
		Mesh(unsigned int VAO, unsigned int VBO, unsigned int EBO)
			: m_VBO(VBO), m_VAO(VAO), m_EBO(EBO) {
		}
		Mesh(Cle::Gfx::GenericMesh g_Mesh)
		{
			indexCount = g_Mesh.getIndices().size();
			vertexCount = g_Mesh.getVertices().size();
			VBO vbo(g_Mesh.getVertices());
			VAO vao(vbo.ID);
			EBO ebo(g_Mesh.getIndices());

			vao.LinkAttrib(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
			vao.LinkAttrib(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 3);
			vao.LinkAttrib(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 5);
			m_VAO = vao.ID;
			m_VBO = vbo.ID;
			m_EBO = ebo.ID;
			g_Mesh.Dirty = Cle::Gfx::GenericMesh::VERTICES;
		}
		void reupload(Cle::Gfx::GenericMesh g_Mesh) {
			indexCount = g_Mesh.getIndices().size();
			vertexCount = g_Mesh.getVertices().size();
			VBO vbo(g_Mesh.getVertices());
			VAO vao(vbo.ID);
			EBO ebo(g_Mesh.getIndices());

			vao.LinkAttrib(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
			vao.LinkAttrib(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 3);
			vao.LinkAttrib(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 5);
			m_VAO = vao.ID;
			m_VBO = vbo.ID;
			m_EBO = ebo.ID;
		}
	};
	
}

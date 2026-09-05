#include "OpenGLMesh.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
using namespace Cle::OPENGL;
using namespace Cle::Gfx;
void Cle::OPENGL::Mesh::draw()
{
	
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	if (indexCount == 0) {
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
	else {
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	}
	
}

void Cle::OPENGL::Mesh::draw(unsigned int nebo, unsigned int count)
{
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nebo);

	if (indexCount == 0) {
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
	else {
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
	}

}

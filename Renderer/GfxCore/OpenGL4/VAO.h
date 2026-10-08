#pragma once
#include "GfxBase.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
namespace Cle::OPENGL
{
	struct VAO
	{
		GLuint ID{};
		VAO(GLuint VBO);
		void LinkAttrib(int location, int attribs, GLenum dataType, unsigned char normalize, int stride, int pointer);
	};
}
#pragma once
#include "GfxBase.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
namespace Cle::OPENGL
{
	struct EBO
	{
		GLuint ID{};
		EBO(const std::vector<unsigned int>& indices);
	};
}
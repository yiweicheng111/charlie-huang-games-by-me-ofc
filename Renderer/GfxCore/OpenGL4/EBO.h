#pragma once
#include "GfxBase.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
namespace Cle::Gfx::OPENGL43
{
	struct EBO
	{
		GLuint ID{};
		EBO(std::vector<unsigned int>& indices);
	};
}
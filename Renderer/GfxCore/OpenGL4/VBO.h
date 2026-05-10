#pragma once
#include "GfxBase.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
namespace Cle::Gfx::OPENGL43
{
	struct VBO
	{
		GLuint ID{};
		VBO(const std::vector<Cle::Gfx::Vertex>& vertices);
	};
}
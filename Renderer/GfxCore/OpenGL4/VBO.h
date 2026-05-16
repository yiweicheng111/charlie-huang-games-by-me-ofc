#pragma once
#include "GfxBase.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
namespace Cle::OPENGL
{
	struct VBO
	{
		GLuint ID{};
		VBO(const std::vector<Cle::Gfx::Vertex>& vertices);
	};
}
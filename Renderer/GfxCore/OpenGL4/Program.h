#pragma once
#include "GfxBase.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
namespace Cle::Gfx::OPENGL43
{
	struct Program
	{
		GLuint ID{};
		Program(std::string vertexShader, std::vector<std::string>& fragmentShaders);
	};
}
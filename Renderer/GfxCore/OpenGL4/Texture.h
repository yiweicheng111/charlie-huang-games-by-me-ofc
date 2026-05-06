#pragma once
#include "OpenGL4/VAO.h"
#include "stb/stb_image.h"
namespace Cle::Gfx::OPENGL43 
{

	struct Texture
	{
		GLuint ID =-1;
		Texture() = default;
		Texture(unsigned int ID) : ID(ID) {}
		Texture(const char* path);
	};
}
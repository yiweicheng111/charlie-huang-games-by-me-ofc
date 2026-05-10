#pragma once
#include "GfxBase.h"
#include "OpenGL4/VAO.h"
#include "OpenGL4/Program.h"
#include "OpenGL4/Texture.h"
#include <optional>
#include "OpenGL4/Shader.h"
namespace Cle::Gfx
{
	struct Material
	{
		bool usesColorMap = true;
		Cle::Gfx::OPENGL43::Shader m_Shader;
		glm::vec3 Color;
		Cle::Gfx::OPENGL43::Texture colorMap;
		Material(Cle::Gfx::OPENGL43::Program Program) : m_Shader(Program.ID) {}
		Material(GLuint Program) : m_Shader(Program) {}

	};
}

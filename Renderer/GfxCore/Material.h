#pragma once
#include "GfxBase.h"
#include "OpenGL4/VAO.h"
#include "OpenGL4/Program.h"
#include "OpenGL4/Texture.h"
#include <optional>
#include "OpenGL4/Shader.h"

namespace Cle::Gfx
{
	class Material
	{
	private:
		glm::vec3 Color = glm::vec3(1.0f);
		Cle::OPENGL::Texture colorMap;
	public:
		bool dirty = false;
		
		void setColor(glm::vec3 _Color)
		{
			dirty = true;
			Color = _Color;
		}
		glm::vec3 getColor() const
		{
			return Color;
		}
		void setColorMap(Cle::OPENGL::Texture _Tex)
		{
			dirty = true;
			colorMap = _Tex;
		}
		Cle::OPENGL::Texture getColorMap() const
		{
			return colorMap;
		}

		bool usesColorMap = true;
		Cle::OPENGL::Shader m_Shader;
		Material(Cle::OPENGL::Program Program) : m_Shader(Program.ID) {}
		Material(GLuint Program) : m_Shader(Program) {}
		Material() : m_Shader(0) {}
	};
}

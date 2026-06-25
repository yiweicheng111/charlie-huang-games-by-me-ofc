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
		std::shared_ptr<ITexture> colorMap;
	public:
		bool gpuUploaded = false;
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
		void setColorMap(std::shared_ptr<ITexture> _Tex)
		{
			dirty = true;
			colorMap = _Tex;
		}
		std::shared_ptr<ITexture> getColorMap() const
		{
			return colorMap;
		}
		bool usesColorMap = false;
		std::string savedColorMap;
		std::string savedSpecularMap;
		Cle::OPENGL::Shader m_Shader;
		Material(Cle::OPENGL::Program Program) : m_Shader(Program.ID) {}
		Material(GLuint Program) : m_Shader(Program) {}
		Material() : m_Shader(0) {}
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(Color, colorMap);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(Color, colorMap);
			dirty = true;
		}
	};
}

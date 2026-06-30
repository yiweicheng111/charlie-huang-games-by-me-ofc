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
		std::shared_ptr<ITexture> colorMap = nullptr;
	public:
		std::string name{};
		//bool gpuUploaded = false;
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
	//	Material(Cle::OPENGL::Program Program) : m_Shader(Program.ID) {}
	//	Material(GLuint Program) : m_Shader(Program) {}
		Material() = default;
		template <class Archive>
		void save(Archive& ar) const
		{
			if (colorMap == nullptr)
			{
				ar(Color, "");
			}
			else
			{
				ar(Color, colorMap->getPath());

			}
		}
		template <class Archive>
		void load(Archive& ar)
		{
			std::string colorMapPath{};
			ar(Color, colorMapPath);
			dirty = true;
		}
	};
}

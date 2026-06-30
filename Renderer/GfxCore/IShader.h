#pragma once
#include "ITexture.h"
namespace Cle
{
	class IShader
	{
	public:

		virtual void Bind()=0;
		virtual void setMat4(std::string uniform, glm::mat4 arg)=0;
		virtual void setFloat(std::string uniform, float arg)=0;
		virtual void setVec3(std::string uniform, glm::vec3 arg)=0;
		virtual void setVec4(std::string uniform, glm::vec4 arg)=0;
		virtual void setInt(std::string uniform, int arg)=0;
		virtual void UniformLights(std::shared_ptr<Cle::Gfx::ITexture> lightTexel) = 0;
	};
}
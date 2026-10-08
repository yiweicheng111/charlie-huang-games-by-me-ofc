#pragma once
#include "OpenGL4/VAO.h"
#include "Light.h"
#include <unordered_map>
#include "IShader.h"
#include "Texture.h"
namespace Cle::OPENGL
{
	class Shader : public IShader
	{
	public:
		std::unordered_map<std::string, int> uniformsCache;
		GLuint programID;
		Shader(GLuint programID) :programID(programID) {}
		virtual void Bind() override;
		virtual void setMat4(std::string uniform, glm::mat4 arg) override;
		virtual void setFloat(std::string uniform, float arg) override;
		virtual void setVec3(std::string uniform, glm::vec3 arg) override;
		virtual void setVec4(std::string uniform, glm::vec4 arg) override;
		virtual void setInt(std::string uniform, int arg) override;
		virtual void UniformLights(std::shared_ptr<Cle::Gfx::ITexture> lightTexel) override {};
		virtual void UniformLights(int lightTexel, int lightMapSlot);

	};
}
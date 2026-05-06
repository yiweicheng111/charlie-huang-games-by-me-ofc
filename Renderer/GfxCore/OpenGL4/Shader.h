#pragma once
#include "OpenGL4/VAO.h"
#include <unordered_map>
namespace Cle::Gfx::OPENGL43
{
	struct Shader
	{
		std::unordered_map<std::string, int> uniformsCache;
		GLuint programID;
		Shader(GLuint programID) :programID(programID) {}
		void Bind();
		void setMat4(std::string uniform, glm::mat4 arg);
		void setFloat(std::string uniform, float arg);
		void setVec3(std::string uniform, glm::vec3 arg);
		void setVec4(std::string uniform, glm::vec4 arg);
		void setInt(std::string uniform, int arg);

	};
}
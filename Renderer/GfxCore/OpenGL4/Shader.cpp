#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
void Cle::Gfx::OPENGL43::Shader::Bind()
{
	glUseProgram(programID);
}

void Cle::Gfx::OPENGL43::Shader::setMat4(std::string uniform, glm::mat4 arg)
{
	int location{};
	if (uniformsCache.contains(uniform)) {
		location = uniformsCache[uniform];
	}
	else {
		location = glGetUniformLocation(programID, uniform.c_str());
		uniformsCache[uniform] = location;
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(arg));
}



void Cle::Gfx::OPENGL43::Shader::setFloat(std::string uniform, float arg)
{
	int location{};
	if (uniformsCache.contains(uniform)) {
		location = uniformsCache[uniform];
	}
	else {
		location = glGetUniformLocation(programID, uniform.c_str());
		uniformsCache[uniform] = location;
	}
	glUniform1f(location, arg);
	
}

void Cle::Gfx::OPENGL43::Shader::setVec3(std::string uniform, glm::vec3 arg)
{
	int location{};
	if (uniformsCache.contains(uniform)) {
		location = uniformsCache[uniform];
	}
	else {
		location = glGetUniformLocation(programID, uniform.c_str());
		uniformsCache[uniform] = location;
	}
	glUniform3f(location, arg.x, arg.y, arg.z);
}

void Cle::Gfx::OPENGL43::Shader::setVec4(std::string uniform, glm::vec4 arg)
{
	int location{};
	if (uniformsCache.contains(uniform)) {
		location = uniformsCache[uniform];
	}
	else {
		location = glGetUniformLocation(programID, uniform.c_str());
		uniformsCache[uniform] = location;
	}
	glUniform4f(location, arg.x, arg.y, arg.z,arg.w);
}

void Cle::Gfx::OPENGL43::Shader::setInt(std::string uniform, int arg)
{
	int location{};
	if (uniformsCache.contains(uniform)) {
		location = uniformsCache[uniform];
	}
	else {
		location = glGetUniformLocation(programID, uniform.c_str());
		uniformsCache[uniform] = location;
	}
	glUniform1i(location, arg);
}

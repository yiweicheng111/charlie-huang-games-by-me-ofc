#include "EBO.h"
#include "VAO.h"
#include "VBO.h"
#include "Program.h"
#include <iostream>
using namespace Cle::Gfx;
Cle::OPENGL::VBO::VBO(const std::vector<Vertex>& vertices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
}

Cle::OPENGL::EBO::EBO(const std::vector<unsigned int>& indices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
}

Cle::OPENGL::VAO::VAO(GLuint VBO)
{
	glGenVertexArrays(1, &ID);
	glBindVertexArray(ID);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void Cle::OPENGL::VAO::LinkAttrib(int location, int attribs, GLenum dataType, unsigned char normalize, int stride, int pointer)
{
	glVertexAttribPointer(location, attribs, dataType, normalize, stride, (void*)(pointer*sizeof(float)));
	glEnableVertexAttribArray(location);
}

Cle::OPENGL::Program::Program(std::string vertexShaderPath, std::vector<std::string>& fragmentShadersPath)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::string read = readFile(vertexShaderPath);
	const char* cread = read.c_str();

	glShaderSource(vertexShader, 1, &cread, NULL);
	glCompileShader(vertexShader);
	char errorLog[512];
	int success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cout << errorLog << std::endl;
	}
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fread = "";
	for (std::string i : fragmentShadersPath) {
		std::cout << i;
		fread += readFile(i) + "\n";
	}
	const char* cfread = fread.c_str();
	glShaderSource(fragmentShader, 1, &cfread, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cout << errorLog << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}
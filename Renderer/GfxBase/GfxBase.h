#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace Cle::Gfx
{

	std::string readFile(std::string path);
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 texCoord;
		glm::vec3 Normal;
		Vertex() = default;
		Vertex(glm::vec3 Position, glm::vec2 texCoord, glm::vec3 Normal) : Position(Position), texCoord(texCoord), Normal(Normal) {}
	};

	enum Pipeline : int
	{
		OPENGL,
		OPENGLMAC,
		DIRECTX12,
		OPENES
	};
	inline Pipeline G_PIPE_LINE = Cle::Gfx::Pipeline::OPENGL;
};

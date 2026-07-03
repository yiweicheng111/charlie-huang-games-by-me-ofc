#pragma once
#include <glm/glm.hpp>
namespace Cle::Gfx
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 texCoord;
		glm::vec3 Normal;
		bool operator==(const Vertex& other) const {
			return (Position == other.Position) && (texCoord == other.texCoord) && (Normal == other.Normal);
		}
		Vertex() = default;
		Vertex(glm::vec3 Position, glm::vec2 texCoord, glm::vec3 Normal) : Position(Position), texCoord(texCoord), Normal(Normal) {}
	};
}
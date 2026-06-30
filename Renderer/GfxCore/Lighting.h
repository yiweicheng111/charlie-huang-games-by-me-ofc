#pragma once
#include "glm/glm.hpp"
#include "Components/Components.h"
namespace Cle
{
	class Lighting
	{
	private:

	public:
		Lighting(const Lighting&) = delete;
		Lighting& operator=(const Lighting&) = delete;

		Lighting(Lighting&&) = delete;
		Lighting& operator=(Lighting&&) = delete;

		static Lighting& getInstance()
		{
			static Lighting instance{};
			return instance;
		}

		float fogStart = 1.0f;
		float fogEnd = 1e6;
		float ambient = 0.5f;
		glm::vec3 ambientColor = glm::vec3(1.0f);
		glm::vec3 backgroundColor = glm::vec3(0.8f, 0.8f, 0.9f);
		glm::vec3 sunDirection{1,-1.0,0.3 };
		Cle::Components::CubeMapTexture skybox;

	private:
		Lighting() {}
		~Lighting() {}
	};
}
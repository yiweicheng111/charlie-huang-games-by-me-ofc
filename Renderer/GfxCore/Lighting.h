#pragma once
#include "glm/glm.hpp"
#include "CharlieEngine/Components.h"
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

		float fogStart = 1e3;
		float fogEnd = 1e4;
		float ambient = 0.3f;

		glm::vec3 ambientColor = glm::vec3(0.9,0.5,0.5);
		glm::vec3 sunDirection{0.5,-0.3,0.7};

		glm::vec3 skyBottom = glm::vec3(1, 0.8, 0.8);
		glm::vec3 skyTop = glm::vec3(0.6, 0.7, 1);
		glm::vec3 backgroundColor = skyBottom;

		Cle::Components::CubeMapTexture skybox;
		std::shared_ptr<ITexture> skyboxTex;
		bool usesSkybox = false;
	private:
		Lighting() {}
		~Lighting() {}
	};
}
#pragma once
#include <glm/glm.hpp>
#include "OpenGL4/Texture.h"
#include "../Light.h"
#include "IShader.h"
namespace Cle::OPENGL {
	class LightBuffer
	{
	public:
		GLuint lightTexture;
		bool sizeDirty = false;
		bool dataDirty = false;
		std::shared_ptr<Cle::OPENGL::Shader> shader;
		std::vector<float> LightFloatVector;
		std::vector<Cle::Gfx::Light> LightVector;
		LightBuffer(std::shared_ptr<Cle::OPENGL::Shader> _shader) : shader(_shader)
		{
			glGenTextures(1, &lightTexture);
			glBindTexture(GL_TEXTURE_2D, lightTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,2, LightFloatVector.size(), 0, GL_RGBA, GL_FLOAT, LightFloatVector.data());
		}

		int addLight(Cle::Gfx::Light light)
		{
			LightVector.push_back(light);
			sizeDirty = true;
			return LightVector.size()-1;
		}
		void removeLight(int index)
		{
			LightVector.erase(LightVector.begin()+index);
			bool sizeDirty = true;
			bool dataDirty = true;
		}
		void updateLight(int index, Cle::Gfx::Light light)
		{
			LightVector[index] = light;
			int foffset = index * 8;

			LightFloatVector[foffset] = light.color.x;
			LightFloatVector[foffset +1] = light.color.y;
			LightFloatVector[foffset +2] = light.color.z;
			LightFloatVector[foffset +3] = light.radius;


			LightFloatVector[foffset +4] = light.position.x;
			LightFloatVector[foffset +5] = light.position.y;
			LightFloatVector[foffset +6] = light.position.z;
			LightFloatVector[foffset +7] = light.m_Type;
			dataDirty = true;
		}
		void updateVector()
		{
			glBindTexture(GL_TEXTURE_2D, lightTexture);
			if (sizeDirty)
			{

				LightFloatVector.clear();
				for (auto& light : LightVector)
				{
					LightFloatVector.push_back(light.color.x);
					LightFloatVector.push_back(light.color.y);
					LightFloatVector.push_back(light.color.z);
					LightFloatVector.push_back(light.radius);

					LightFloatVector.push_back(light.position.x);
					LightFloatVector.push_back(light.position.y);
					LightFloatVector.push_back(light.position.z);
					LightFloatVector.push_back(light.m_Type);
				}
				sizeDirty = false;
				dataDirty = false;
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2, LightVector.size(),0, GL_RGBA, GL_FLOAT, LightFloatVector.data());
			}
			if (dataDirty)
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 2, LightVector.size(), GL_RGBA, GL_FLOAT, LightFloatVector.data());
				dataDirty = false;
			}
		}
	};
}
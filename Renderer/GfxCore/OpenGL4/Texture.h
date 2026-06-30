#pragma once
#include "OpenGL4/VAO.h"
#include "stb/stb_image.h"
#include "../ITexture.h"
#include <cereal/types/polymorphic.hpp>
#include <iostream>
namespace Cle::OPENGL 
{
	class Renderer;
	class Texture : public Cle::Gfx::ITexture
	{
	private:
		GLuint ID = -1;
	public:

		~Texture() {
			glDeleteTextures(1, &ID);
		}
		Texture(std::string path);
		Texture(std::string path,GLenum target);
		Texture() = default;
		Texture(unsigned int ID) : ID(ID) {}
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(cereal::base_class<Cle::Gfx::ITexture>(this));
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(cereal::base_class<Cle::Gfx::ITexture>(this));

		}
		virtual void bind(unsigned int slot) const override {
			glBindTexture(GL_TEXTURE_2D, ID);
		}
		virtual int getID() const override
		{
			return ID;
		}
		virtual void setPath(std::string nPath) override
		{
			path = nPath;
			glBindTexture(GL_TEXTURE_2D, ID);
			int width, height, channels;
			unsigned char* data = stbi_load(nPath.c_str(), &width, &height,&channels,0);
			if (!data) {
				loaded = false;
				std::cerr << "image " << path << " not loaded\n";
				return;
			}
			GLenum format = GL_RGB;
			if (channels == 1) format = GL_RED;
			if (channels == 4) format = GL_RGBA;
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_TEXTURE_2D, data);
			loaded = true;
		}
		virtual void setPath(std::string nPath,GLenum target) 
		{
			path = nPath;
			glBindTexture(target, ID);
			int width, height, channels;
			unsigned char* data = stbi_load(nPath.c_str(), &width, &height, &channels, 0);
			if (!data) {
				loaded = false;
				std::cerr << "image " << path << " not loaded\n";
				return;
			}
			GLenum format = GL_RGB;
			if (channels == 1) format = GL_RED;
			if (channels == 4) format = GL_RGBA;
			glTexSubImage2D(target, 0, 0, 0, width, height, format, target, data);
			loaded = true;
			
		}
	};
}

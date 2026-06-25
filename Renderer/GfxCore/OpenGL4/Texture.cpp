#include "Texture.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb/stb_image.h"
#include <iostream>
using namespace Cle::OPENGL;
Texture::Texture(std::string path) {
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	int width, height, channels;
	unsigned char* bytes = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!bytes) {
		std::cerr << "image " << path << " not loaded\n";
		loaded = false;
		return;
	}
	GLenum format;
	switch (channels)
	{
	case 1: channels = GL_RED; break;
	case 3: channels = GL_RGB; break;
	case 4: channels = GL_RGBA; break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, GL_NONE, channels, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(bytes);
	loaded = true;
}
Texture::Texture(std::string path,GLenum target) {
	glGenTextures(1, &ID);
	glBindTexture(target, ID);
	int width, height, channels;
	unsigned char* bytes = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!bytes) {
		std::cerr << "image " << path << " not loaded\n";
		loaded = false;
		return;
	}
	GLenum format;
	switch (channels)
	{
	case 1: channels = GL_RED; break;
	case 3: channels = GL_RGB; break;
	case 4: channels = GL_RGBA; break;
	}

	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexImage2D(target, 0, channels, width, height, GL_NONE, channels, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(target);
	stbi_image_free(bytes);
	loaded = true;
}
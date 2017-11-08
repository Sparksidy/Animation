#pragma once

#include "stb_image.h"
#include <string>

#include <glew.h>


class Textures
{
public:
	Textures(GLenum TextureTarget, const std::string& FileName);

	bool Load();

	void Bind(GLenum TextureUnit);

private:
	std::string m_fileName;
	GLenum m_textureTarget;
	GLuint m_textureObj;
	
};

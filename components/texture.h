#pragma once



#include <GL\glew.h>
#include <glm\gtc\matrix_transform.hpp>
#include "shader.h"
#include "meshObject.h"



class Texture
{
public:
	Texture();
	Texture(const char* fileLoc);

	bool LoadTexture();
	bool LoadTextureA();

	void UseTexture(GLenum textureUnit);
	void ClearTexture();
	~Texture();

private:
	GLuint textureID;
	int width, height, bitDepth;
	const char* fileLocation;
};


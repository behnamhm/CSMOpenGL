#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "shadowSystem.h"

class Light
{
public:
	Light();
	Light(GLuint shadowWidth, GLuint shadowHeight,
		GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity);

	ShadowSystem* getShadowMap() { return shadowSystem; }

	~Light();

protected:
	glm::vec3 colour;
	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;

	glm::mat4 lightProj;

	ShadowSystem* shadowSystem;
};


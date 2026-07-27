#pragma once

#include <vector>
#include <string>

#include <GL\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <filesystem>


#include "MeshObject.h"
#include "Shader.h"

class Skybox
{
public:
	Skybox();

	Skybox(std::vector<std::string> faceLocations);

	void DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
	void LoadHdrEnvironment(unsigned int& hdrTexture);

	~Skybox();

private:
	Mesh skyMesh;
	Shader* skyShader;

	GLuint textureId;
	GLuint uniformProjection, uniformView;
};


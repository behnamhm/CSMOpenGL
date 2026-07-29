#pragma once

#include <vector>
#include <memory>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <GL\glew.h>

#include "lights/shadowSystem.h"


class Shader;
class Debug
{
public:
	ShadowSystem shadowSystem;
	void drawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader);

private:
	std::vector<GLuint> visualizerVAOs;
	std::vector<GLuint> visualizerVBOs;
	std::vector<GLuint> visualizerEBOs;

};
#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

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
	void drawCascadeflatVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader);
	std::vector<glm::vec4> getFrustumFlatCornersWorldSpace(const glm::mat4& projview);

private:
	std::vector<GLuint> visualizerVAOs;
	std::vector<GLuint> visualizerVBOs;
	std::vector<GLuint> visualizerEBOs;

	std::vector<GLuint> flatvisualizerVAOs;
	std::vector<GLuint> flatvisualizerVBOs;
	std::vector<GLuint> flatvisualizerEBOs;

};
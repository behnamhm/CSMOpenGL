#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "components/meshObject.h"
#include "components/texture.h"
#include "components/Buffer.h"
#include "components/inputManager.h"
#include "components/camera.h"
#include "components/Skybox.h"
#include "components/Model.h"
#include "lights/shadowSystem.h"
#include "scene/Debug.h"


class Scene;
class Renderer
{
public:

	void RenderScene(Scene& scene, GLuint uniformModel, GLfloat deltaTime);

	void RenderPass(Scene& scene, GLfloat deltaTime,
					unsigned int& envCubemap, unsigned int& irradianceMap,
					unsigned int&  prefilterMap, unsigned int& brdfLUTTexture,
					std::vector<float>& shadowCascadeLevels, unsigned int& lightDepthMaps);

	void ShadowPass(Scene& scene, GLfloat deltaTime, unsigned int& lightFBO,
					unsigned int& depthMapResolution, std::vector<float>& shadowCascadeLevels,
					unsigned int& matricesUBO);


	GLuint uniformProjection{ 0 }, uniformModel{ 0 }, uniformView{ 0 }, uniformEyePosition{ 0 };
	std::vector<glm::mat4> lightMatricesCache;

	ShadowSystem shadowSystem;
	Debug debug;


	int debugLayer = 0;
	bool showQuad = false;
};
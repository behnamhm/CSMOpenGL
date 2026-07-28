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


class Scene;

class appObject
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

	void drawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader);
	void processInput(Scene& scene, GLFWwindow* window, std::vector<float>& shadowCascadeLevels);
	std::vector<glm::mat4> getLightSpaceMatrices(Scene& scene, std::vector<float>& shadowCascadeLevels,
													float& cameraNearPlane, float& cameraFarPlane);
	glm::mat4 getLightSpaceMatrix(Scene& scene, const float nearPlane, const float farPlane);
	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);
  	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

	GLuint uniformProjection{ 0 }, uniformModel{ 0 }, uniformView{ 0 }, uniformEyePosition{ 0 };
	std::vector<glm::mat4> lightMatricesCache;
	std::vector<GLuint> visualizerVAOs;
	std::vector<GLuint> visualizerVBOs;
	std::vector<GLuint> visualizerEBOs;

	int fb_width;
	int fb_height;
	int debugLayer = 0;
	bool showQuad = false;
	float cameraNear = 0.1f;
	float cameraFar = 500.0f;

};
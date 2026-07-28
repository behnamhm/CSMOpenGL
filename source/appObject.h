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
#include "components/objectTypes.h"


class appObject
{
public:

	void RenderScene(GLuint uniformModel, GLfloat deltaTime,
					std::unordered_map<MeshType, std::unique_ptr<Mesh>>& meshList,
					std::unordered_map<TextureType, std::unique_ptr<Texture>>& textureList,
					std::unordered_map<ModelType, std::unique_ptr<Model>>& modelList, 
					std::unordered_map<ShaderType, std::unique_ptr<Shader>>& shaderList);

	void RenderPass(glm::mat4 camera_view, glm::mat4 projectionMatrix, GLfloat deltaTime,
					std::unordered_map<MeshType, std::unique_ptr<Mesh>>& meshList,
					std::unordered_map<TextureType, std::unique_ptr<Texture>>& textureList,
					std::unordered_map<ModelType, std::unique_ptr<Model>>& modelList, 
					std::unordered_map<ShaderType, std::unique_ptr<Shader>>& shaderList,
					std::vector<std::unique_ptr<DirectionalLight>>& DirectionalLightList,
				    std::vector<std::unique_ptr<PointLight>>& PointLightList,
					unsigned int& envCubemap, unsigned int& irradianceMap,
					unsigned int&  prefilterMap, unsigned int& brdfLUTTexture,
					std::vector<float>& shadowCascadeLevels, unsigned int& lightDepthMaps);

	void ShadowPass(glm::mat4 camera_view, glm::mat4 projectionMatrix, GLfloat deltaTime,
					std::unordered_map<MeshType, std::unique_ptr<Mesh>>& meshList,
					std::unordered_map<TextureType, std::unique_ptr<Texture>>& textureList,
					std::unordered_map<ModelType, std::unique_ptr<Model>>& modelList,
					std::unordered_map<ShaderType, std::unique_ptr<Shader>>& shaderList,
					std::vector<std::unique_ptr<DirectionalLight>>& DirectionalLightList,
					std::vector<std::unique_ptr<PointLight>>& PointLightList,
					unsigned int& envCubemap, unsigned int& irradianceMap,
					unsigned int& prefilterMap, unsigned int& brdfLUTTexture, unsigned int& lightFBO, 
					unsigned int& depthMapResolution, std::vector<float>& shadowCascadeLevels,
					unsigned int& matricesUBO, float& cameraNearPlane, float& cameraFarPlane);

	std::vector<glm::mat4> getLightSpaceMatrices(std::vector<float>& shadowCascadeLevels, 
													float& cameraNearPlane, float& cameraFarPlane);
	glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane);
	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);
  	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
	void drawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader);
	void processInput(GLFWwindow* window, std::vector<float>& shadowCascadeLevels);


	glm::vec3 sphere_phys_pos;
	glm::quat sphere_phys_rot;
	InputManager inputManager;
	Camera camera;

	GLuint uniformProjection{ 0 }, uniformModel{ 0 }, uniformView{ 0 }, uniformEyePosition{ 0 };

	int fb_width;
	int fb_height;
	int debugLayer = 0;
	bool showQuad = false;
	std::vector<glm::mat4> lightMatricesCache;
	std::vector<GLuint> visualizerVAOs;
	std::vector<GLuint> visualizerVBOs;
	std::vector<GLuint> visualizerEBOs;
	float cameraNear = 0.1f;
	float cameraFar = 500.0f;
};
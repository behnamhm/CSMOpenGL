#pragma once

#include <vector>
#include <memory>


#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "components/meshObject.h"
#include "components/texture.h"
#include "components/inputManager.h"
#include "components/camera.h"
#include "components/Skybox.h"
#include "components/Model.h"


class appObject
{
public:
	void RenderScene(GLuint uniformModel, GLfloat deltaTime,
					std::vector<std::unique_ptr<Mesh>> &meshList,
					std::vector<std::unique_ptr<Texture>>& textureList,
					std::vector<std::unique_ptr<Model>>& modelList,
					std::vector<std::unique_ptr<Shader>>& shaderList);

	void RenderPass(glm::mat4 camera_view, glm::mat4 projectionMatrix, GLfloat deltaTime,
					std::vector<std::unique_ptr<Mesh>>& meshList,
					std::vector<std::unique_ptr<Texture>>& textureList,
					std::vector<std::unique_ptr<Model>>& modelList,
					std::vector<std::unique_ptr<Shader>> &shaderList, 
					std::vector<std::unique_ptr<DirectionalLight>>& DirectionalLightList,
				    std::vector<std::unique_ptr<PointLight>>& PointLightList,
					unsigned int& envCubemap, unsigned int& irradianceMap,
					unsigned int&  prefilterMap, unsigned int& brdfLUTTexture);

	~appObject();

	glm::vec3 sphere_phys_pos;
	glm::quat sphere_phys_rot;
	InputManager inputManager;
	Camera camera;

	GLuint uniformProjection{ 0 }, uniformModel{ 0 }, uniformView{ 0 }, uniformEyePosition{ 0 };


};
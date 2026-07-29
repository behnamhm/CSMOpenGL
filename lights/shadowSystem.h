#pragma once

#include <stdio.h>
#include <GL\glew.h>
#include <vector>
#include <memory>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>



class Scene;
class ShadowSystem
{
public:
	ShadowSystem();

	virtual bool Init(unsigned int width, unsigned int height);

	virtual void Write();

	virtual void Read(GLenum TextureUnit);

	GLuint GetShadowWidth() { return shadowWidth; }
	GLuint GetShadowHeight() { return shadowHeight; }
	std::vector<glm::mat4> getLightSpaceMatrices(Scene& scene, std::vector<float>& shadowCascadeLevels,
												float& cameraNearPlane, float& cameraFarPlane);
	glm::mat4 getLightSpaceMatrix(Scene& scene, const float nearPlane, const float farPlane);
	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);
	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
	int fb_width;
	int fb_height;
	~ShadowSystem();
protected:
	GLuint FBO, shadowMap;
	GLuint shadowWidth, shadowHeight;

};
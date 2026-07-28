#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include <GL\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>


#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"



class Shader
{
public:
	Shader();

	void CreateFromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);
	void CreateFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation);

	void Validate();

	std::string ReadFile(const char* fileLocation);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetViewLocation();
	GLuint GetAmbientIntensityLocation();
	GLuint GetAmbientColourLocation();
	GLuint GetDiffuseIntensityLocation();
	GLuint GetDirectionLocation();
	GLuint GetSpecularIntensityLocation();
	GLuint GetShininessLocation();
	GLuint GetEyePositionLocation();
	GLuint GetOmniLightPosLocation();
	GLuint GetFarPlaneLocation();


	void SetDirectionalLight(DirectionalLight* dLight);
	void SetPointLights(std::vector<std::unique_ptr<PointLight>>& PointLightList, 
						unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	void SetTexture(GLuint textureUnit);
	void SetSpecular(GLuint textureUnit);
	void SetRoughnessMap(GLuint textureUnit);
	void SetMetalMap(GLuint textureUnit);
	void SetCascadeCount(GLuint textureUnit);
	void SetCascadeDistance(const std::string& name, float value);
	void SetDirectionalLightTransform(glm::mat4* lTransform);
	void SetLightMatrices(std::vector<glm::mat4> lightMatrices);
	GLuint GetUseTextureLocation();
	GLuint GetMetallic();
	void SetUseTexture(bool value);
	void SetMetallic(float value);
	void SetRoughness(float value);
	void SetEnvironmentMap(float value);
	void SetEquirectangularMap(float value);
	void SetIrradianceMap(int value);
	void SetPrefilterMap(int value);
	void SetShadowMp(int value);
	void SetLayer(int value);
	void SetColors(const glm::vec4& value);
	void SetBrdfLUT(int value);
	void SetProjection(glm::mat4& projectionMatrix);
	void SetView(glm::mat4& viewMatrix);


	void UseShader();
	void ClearShader();

	~Shader();

private:
	int pointLightCount;
	int spotLightCount;



	GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformEyePosition,
		uniformSpecularIntensity, uniformShininess,
		uniformTexture, uniformDirectionalShadowMap, unifromTexture_specular,
		uniformDirectionalLightTransform,
		uniformOmniLightPos, uniformFarPlane, uniformAlbedo, uniformMetalic, uniformRoughness, uniformAo;

	GLuint uniformLightMatrices[6];

	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDirection;
	} uniformDirectionalLight;

	GLuint uniformPointLightCount;

	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	} uniformPointLight[3];


	struct {
		GLuint shadowMap;
		GLuint farPlane;
	} uniformOmniShadowMap[3];

	void CompileShader(const char* vertexCode, const char* fragmentCode);
	void CompileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode);
	void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);

	void CompileProgram();
};


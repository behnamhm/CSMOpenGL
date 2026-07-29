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


	std::string ReadFile(const char* fileLocation);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetViewLocation();
	GLuint GetEyePositionLocation();
	void SetDirectionalLight(DirectionalLight* dLight);
	void SetPointLights(std::vector<std::unique_ptr<PointLight>>& PointLightList, 
						unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	void SetTexture(GLuint textureUnit);
	void SetDirectionalLightTransform(glm::mat4* lTransform);

	void Shader::setUniform(const std::string& name, bool value);
	void Shader::setUniform(const std::string& name, int value);
	void Shader::setUniform(const std::string& name, float value);
	void Shader::setUniform(const std::string& name, const glm::vec2& value);
	void Shader::setUniform(const std::string& name, float x, float y);
	void Shader::setUniform(const std::string& name, const glm::vec3& value);
	void Shader::setUniform(const std::string& name, float x, float y, float z);
	void Shader::setUniform(const std::string& name, const glm::vec4& value);
	void Shader::setUniform(const std::string& name, float x, float y, float z, float w);
	void Shader::setUniform(const std::string& name, const glm::mat2& mat);
	void Shader::setUniform(const std::string& name, const glm::mat3& mat);
	void Shader::setUniform(const std::string& name, const glm::mat4& mat);

	void UseShader();
	void ClearShader();
	void Validate();
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


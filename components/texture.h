#pragma once



#include <GL\glew.h>
#include <glm\gtc\matrix_transform.hpp>
#include "shader.h"
#include "meshObject.h"



class Texture
{
public:
	Texture();
	Texture(const char* fileLoc);

	bool LoadTexture();
	bool LoadTextureA();

	void UseTexture(GLenum textureUnit);
	void ClearTexture();
	void CaptureFrameBuffer(Shader& shader, Mesh & cube, unsigned int& captureFBO, unsigned int& captureRBO,
							unsigned int& hdrTexture, unsigned int& envCubemap,
							glm::mat4& captureProjection, std::vector<glm::mat4>& captureViews);
	void IrradianceFrameBuffer(Shader& shader, Mesh& cube, unsigned int& captureFBO, unsigned int& captureRBO,
								unsigned int& irradianceMap, unsigned int& envCubemap, 
								glm::mat4& captureProjection, std::vector<glm::mat4>& captureViews);
	void PrefilterFrameBuffer(Shader& shader, Mesh& cube, unsigned int& captureFBO, unsigned int& captureRBO,
								unsigned int& prefilterMap, unsigned int& envCubemap,
								glm::mat4& captureProjection, std::vector<glm::mat4>& captureViews);
	void BRDFLutFrameBuffer(Shader& shader, Mesh& quad, unsigned int& captureFBO, unsigned int& captureRBO,
							unsigned int& prefilterMap);
	void SetupCubeMap(unsigned int& envCubemap);

	~Texture();

private:
	GLuint textureID;
	int width, height, bitDepth;
	const char* fileLocation;
};


#pragma once


#include <string>
#include <iostream>


#include "appObject.h"
#include "components/shader.h"
#include "components/Skybox.h"
#include "components/window.h"
#include "lights/DirectionalLight.h"
#include "lights/PointLight.h"


class Application
{
public:
    void Init();
    void Run();

private:

    float cameraNearPlane = 0.1f;
    float cameraFarPlane = 500.0f;
    std::vector<float> shadowCascadeLevels{ cameraFarPlane / 50.0f, 
                                            cameraFarPlane / 25.0f, 
                                            cameraFarPlane / 10.0f, 
                                            cameraFarPlane / 2.0f };


    glm::mat4 captureProjection;
    std::vector<glm::mat4> captureViews;
    appObject appObj;
    Buffer Buffer;
    Skybox skybox;
    DirectionalLight mainLight;
    GLfloat deltaTime = 0.0f;
    GLfloat lastTime = 0.0f;

    GLFWwindow* thisWindow = nullptr;
    Window window;

    int width = 0;
    int height = 0;
    unsigned int pointLightCount = 0;
    unsigned int depthMapResolution = 4096;
    unsigned int captureFBO, captureRBO, envCubemap, hdrTexture, irradianceMap, 
                prefilterMap, brdfLUTTexture, matricesUBO, lightFBO, lightDepthMaps;

    void CreateShader(ShaderType type, const char* vertexLocation, const char* fragmentLocation)
    {
        appObj.shaderList.emplace(type, std::make_unique<Shader>());
        appObj.shaderList[type]->CreateFromFiles(vertexLocation, fragmentLocation);
    }

    void CreateShader(ShaderType type, const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation)
    {
        appObj.shaderList.emplace(type, std::make_unique<Shader>());
        appObj.shaderList[type]->CreateFromFiles(vertexLocation, geometryLocation, fragmentLocation);
    }

    void CreateMesh(MeshType type, std::string ObjName)
    {
        appObj.meshList.emplace(type, std::make_unique<Mesh>());
        appObj.meshList[type]->DrawObject(ObjName);
    }

    void CreateTexture(TextureType type, const char* path)
    {
        appObj.textureList.emplace(type, std::make_unique<Texture>());
        *appObj.textureList[type] = Texture(path);
        appObj.textureList[type]->LoadTexture();
    }
    void CreateTextureA(TextureType type, const char* path)
    {
        appObj.textureList.emplace(type, std::make_unique<Texture>());
        *appObj.textureList[type] = Texture(path);
        appObj.textureList[type]->LoadTextureA();
    }

    void CreateModel(ModelType type, const std::string& fileName)
    {
        appObj.modelList.emplace(type, std::make_unique<Model>());
        appObj.modelList[type]->LoadModel(fileName);
    }

};
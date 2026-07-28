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

    std::unordered_map<ShaderType, std::unique_ptr<Shader>> shaderList;
    std::unordered_map<MeshType, std::unique_ptr<Mesh>> meshList;
    std::unordered_map<TextureType, std::unique_ptr<Texture>> textureList;
    std::unordered_map<ModelType, std::unique_ptr<Model>> modelList;
    std::vector<std::unique_ptr<DirectionalLight>> DirectionalLightList;
    std::vector<std::unique_ptr<PointLight>> PointLightList;
    std::vector<std::string> faces;

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
        shaderList.emplace(type, std::make_unique<Shader>());
        shaderList[type]->CreateFromFiles(vertexLocation, fragmentLocation);
    }

    void CreateShader(ShaderType type, const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation)
    {
        shaderList.emplace(type, std::make_unique<Shader>());
        shaderList[type]->CreateFromFiles(vertexLocation, geometryLocation, fragmentLocation);
    }

    void CreateMesh(MeshType type, std::string ObjName)
    {
        meshList.emplace(type, std::make_unique<Mesh>());
        meshList[type]->DrawObject(ObjName);
    }

    void CreateTexture(TextureType type, const char* path)
    {
        textureList.emplace(type, std::make_unique<Texture>());
        *textureList[type] = Texture(path);
        textureList[type]->LoadTexture();
    }
    void CreateTextureA(TextureType type, const char* path)
    {
        textureList.emplace(type, std::make_unique<Texture>());
        *textureList[type] = Texture(path);
        textureList[type]->LoadTextureA();
    }

    void CreateModel(ModelType type, const std::string& fileName)
    {
        modelList.emplace(type, std::make_unique<Model>());
        modelList[type]->LoadModel(fileName);
    }

};
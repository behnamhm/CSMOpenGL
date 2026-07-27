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
    ~Application();

private:

    std::unordered_map<ShaderType, std::unique_ptr<Shader>> shaderList;
    std::unordered_map<MeshType, std::unique_ptr<Mesh>> meshList;
    std::vector<std::unique_ptr<Texture>> textureList;
    std::vector<std::unique_ptr<Model>> modelList;
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
    Texture BufferTex;
    Skybox skybox;
    DirectionalLight mainLight;
    GLfloat deltaTime = 0.0f;
    GLfloat lastTime = 0.0f;

    GLFWwindow* thisWindow = nullptr;
    Window window;

    int width = 0;
    int height = 0;
    unsigned int pointLightCount = 0;
    unsigned int captureFBO, captureRBO, envCubemap, hdrTexture, irradianceMap, prefilterMap, brdfLUTTexture;

    void InitShader(ShaderType type)
    {
        shaderList.emplace(type, std::make_unique<Shader>());
    }

    void InitMesh(MeshType type)
    {
        meshList.emplace(type, std::make_unique<Mesh>());
    }


};
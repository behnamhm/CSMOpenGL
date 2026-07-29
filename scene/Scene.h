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
#include "components/camera.h"
#include "components/Skybox.h"
#include "components/Model.h"
#include "components/objectTypes.h"

class Scene
{
public:
    void Register(GLFWwindow* thisWindow);
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


    Camera camera;
    Skybox skybox;
    DirectionalLight mainLight;

    std::unordered_map<ShaderType, std::unique_ptr<Shader>> shaderList;
    std::unordered_map<MeshType, std::unique_ptr<Mesh>> meshList;
    std::unordered_map<TextureType, std::unique_ptr<Texture>> textureList;
    std::unordered_map<ModelType, std::unique_ptr<Model>> modelList;
    std::vector<std::unique_ptr<DirectionalLight>> DirectionalLightList;
    std::vector<std::unique_ptr<PointLight>> PointLightList;
    std::vector<std::string> faces;
    glm::mat4 projectionMatrix{};
    glm::mat4 camera_view{};

    float cameraNearPlane = 0.1f;
    float cameraFarPlane = 500.0f;


private:
    int width = 0;
    int height = 0;


};
#include "Scene.h"

void Scene::Register(GLFWwindow* thisWindow)
{
    // shaders
    CreateShader(ShaderType::Main, "assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    CreateShader(ShaderType::Cubemap, "assets/shaders/cubemapVertex.glsl", "assets/shaders/cubemapFragment.glsl");
    CreateShader(ShaderType::Environment, "assets/shaders/environmentVertex.glsl", "assets/shaders/environmentFragment.glsl");
    CreateShader(ShaderType::Irradiance, "assets/shaders/cubemapVertex.glsl",
        "assets/shaders/irradianceConvolutionFragment.glsl");
    CreateShader(ShaderType::Prefilter, "assets/shaders/cubemapVertex.glsl", "assets/shaders/prefilterFragment.glsl");
    CreateShader(ShaderType::BRDF, "assets/shaders/BRDFVertex.glsl", "assets/shaders/BRDFFragment.glsl");
    CreateShader(ShaderType::ShadowMap, "assets/shaders/shadowMappingVertex.glsl", "assets/shaders/shadowMappingGeometry.glsl",
        "assets/shaders/shadowMappingFragment.glsl");
    CreateShader(ShaderType::DebugDepthQuad, "assets/shaders/debugDepthQuadVertex.glsl",
        "assets/shaders/debugDepthQuadFragment.glsl");
    CreateShader(ShaderType::DebugCascade, "assets/shaders/debugCascadeVertex.glsl",
        "assets/shaders/debugCascadeFragment.glsl");

    // meshes
    CreateMesh(MeshType::Floor, "Floor");
    CreateMesh(MeshType::Cube, "Cube");
    CreateMesh(MeshType::Quad, "Quad");

    // textures
    CreateTexture(TextureType::Checker, "assets/textures/checker.png");
    CreateTexture(TextureType::SpaceshipMetal, "assets/textures/Intergalactic Spaceship_metalness.jpg");
    CreateTextureA(TextureType::CheckerMetal, "assets/textures/checkerMetal.png");
    CreateTextureA(TextureType::CheckerRoughness, "assets/textures/checkerRoughness.png");

    // models
    CreateModel(ModelType::Spaceship, "assets/Models/Intergalactic_Spaceship-(Wavefront).obj");

    // lights
    DirectionalLightList.push_back(std::make_unique<DirectionalLight>(DirectionalLight(2048, 2048,
        1.0f, 1.0f, 1.0f,
        0.5f, 3.5f,
        -0.5f, -5.0f, -2.5f)));

    PointLightList.push_back(std::make_unique<PointLight>(PointLight(1024, 1024,
        0.01f, 200.0f,
        1.0f, 1.0f, 1.0f,
        0.5f, 3.5f,
        4.0f, 5.0f, 2.0f,
        1.0f, 0.09f, 0.132f)));

    PointLightList.push_back(std::make_unique<PointLight>(PointLight(1024, 1024,
        0.01f, 200.0f,
        1.0f, 1.0f, 1.0f,
        0.5f, 3.5f,
        -4.0f, 5.0f, 2.0f,
        1.0f, 0.03f, 0.07f)));

    // projection and view
    glfwGetWindowSize(thisWindow, &width, &height);
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    camera = Camera(glm::vec3(-3.4f, 2.3f, 2.4f), glm::vec3(0.0f, 1.0f, 0.0f), -50.0f, -20.0f, 1.0f, 0.5f);
    projectionMatrix = glm::perspective(glm::radians(90.0f), aspect, cameraNearPlane, cameraFarPlane);
    camera_view = camera.calculateViewMatrix();

    // set uniform values
    shaderList[ShaderType::Main]->UseShader();
    shaderList[ShaderType::Main]->SetProjection(projectionMatrix);
    shaderList[ShaderType::Main]->SetView(camera_view);
    shaderList[ShaderType::Main]->SetIrradianceMap(0);
    shaderList[ShaderType::Main]->SetPrefilterMap(1);
    shaderList[ShaderType::Main]->SetBrdfLUT(2);
    shaderList[ShaderType::Main]->SetShadowMp(3);
    shaderList[ShaderType::Main]->SetRoughnessMap(9);
    shaderList[ShaderType::Main]->SetMetalMap(10);
    shaderList[ShaderType::Environment]->UseShader();
    shaderList[ShaderType::Environment]->SetEnvironmentMap(0);
    shaderList[ShaderType::Environment]->SetProjection(projectionMatrix);
}
#include "Application.h"



void Application::Init()
{

    window.init(appObj, thisWindow);

    InitShader(ShaderType::Main);
    InitShader(ShaderType::Cubemap);
    InitShader(ShaderType::Environment);
    InitShader(ShaderType::Irradiance);
    InitShader(ShaderType::Prefilter);
    InitShader(ShaderType::BRDF);
    InitShader(ShaderType::ShadowMap);

    InitMesh(MeshType::Floor);
    InitMesh(MeshType::Cube);

    InitTexture(TextureType::Checker);
    InitTexture(TextureType::SpaceshipMetal);
    InitTexture(TextureType::CheckerMetal);
    InitTexture(TextureType::CheckerRoughness);

    InitModel(ModelType::Spaceship);


}

void Application::Run()
{
    // window and objects initialisation
    Init();

    // shaders
    shaderList[ShaderType::Main]->CreateFromFiles("assets/shaders/vertex.glsl",
                                                   "assets/shaders/fragment.glsl");

    shaderList[ShaderType::Cubemap]->CreateFromFiles("assets/shaders/cubemapVertex.glsl",
                                                    "assets/shaders/cubemapFragment.glsl");

    shaderList[ShaderType::Environment]->CreateFromFiles("assets/shaders/environmentVertex.glsl",
                                                         "assets/shaders/environmentFragment.glsl");

    shaderList[ShaderType::Irradiance]->CreateFromFiles("assets/shaders/cubemapVertex.glsl",
                                                         "assets/shaders/irradianceConvolutionFragment.glsl");

    shaderList[ShaderType::Prefilter]->CreateFromFiles("assets/shaders/cubemapVertex.glsl",
                                                    "assets/shaders/prefilterFragment.glsl");

    shaderList[ShaderType::BRDF]->CreateFromFiles("assets/shaders/BRDFVertex.glsl",
                                                    "assets/shaders/BRDFFragment.glsl");

    shaderList[ShaderType::ShadowMap]->CreateFromFiles("assets/shaders/shadowMappingVertex.glsl",
                                                        "assets/shaders/shadowMappingGeometry.glsl",
                                                        "assets/shaders/shadowMappingFragment.glsl");

    // meshes
    meshList[MeshType::Floor]->DrawFloor();
    meshList[MeshType::Cube]->DrawCube();

    // textures
    *textureList[TextureType::Checker] = Texture("assets/textures/checker.png");
    textureList[TextureType::Checker]->LoadTexture();

    *textureList[TextureType::SpaceshipMetal] = Texture("assets/textures/Intergalactic Spaceship_metalness.jpg");
    textureList[TextureType::SpaceshipMetal]->LoadTexture();

    *textureList[TextureType::CheckerMetal] = Texture("assets/textures/checkerMetal.png");
    textureList[TextureType::CheckerMetal]->LoadTextureA();

    *textureList[TextureType::CheckerRoughness] = Texture("assets/textures/checkerRoughness.png");
    textureList[TextureType::CheckerRoughness]->LoadTextureA();

    // models
    modelList[ModelType::Spaceship]->LoadModel("assets/Models/Intergalactic_Spaceship-(Wavefront).obj");
   
    // lights
    DirectionalLightList.push_back(std::make_unique<DirectionalLight>(DirectionalLight(2048, 2048,
                                                                1.0f, 1.0f, 1.0f,
                                                                0.5f, 0.6f, -0.5f,
                                                                -5.0f, -2.5f)));

    PointLightList.push_back(std::make_unique<PointLight>(PointLight(1024, 1024,
                                                            0.01f, 200.0f,
                                                            1.0f, 1.0f, 1.0f,
                                                            0.5f, 0.2f, 4.0f,
                                                            5.0f, 2.0f, 1.0f, 0.09f, 0.132f)));

    PointLightList.push_back(std::make_unique<PointLight>(PointLight(1024, 1024,
                                                            0.01f, 200.0f, 1.0f,
                                                            1.0f, 1.0f, 0.5f,
                                                            0.2f, -4.0f,
                                                            5.0f, 2.0f, 1.0f, 0.03f, 0.07f)));

    // projection and view
    glfwGetWindowSize(thisWindow, &width, &height);
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    //appObj.camera = Camera(glm::vec3(0.0f, 6.0f, 9.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 1.0f, 0.5f);
    appObj.camera = Camera(glm::vec3(-3.4f, 2.3f, 2.4f), glm::vec3(0.0f, 1.0f, 0.0f), -50.0f, -20.0f, 1.0f, 0.5f);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), aspect, cameraNearPlane, cameraFarPlane);
    glm::mat4 camera_view = appObj.camera.calculateViewMatrix();

    // Load HDR
    skybox.LoadHdrEnvironment(hdrTexture);

    // Setup Cube map
    BufferTex.SetupCubeMap(envCubemap);

    // Capture framebuffer
    BufferTex.CaptureFrameBuffer(*shaderList[ShaderType::Cubemap], *meshList.at(MeshType::Cube), captureFBO, captureRBO, hdrTexture, envCubemap,
                                  captureProjection, captureViews);

    // Irradiance Map framebuffer
    BufferTex.IrradianceFrameBuffer(*shaderList[ShaderType::Irradiance], *meshList.at(MeshType::Cube), captureFBO, captureRBO, irradianceMap, envCubemap,
                                   captureProjection, captureViews);

    // Prefilter framebuffer
    BufferTex.PrefilterFrameBuffer(*shaderList[ShaderType::Prefilter], *meshList.at(MeshType::Cube), captureFBO, captureRBO, prefilterMap, envCubemap,
                                   captureProjection, captureViews);

    // brdfLUT framebuffer
    BufferTex.BRDFLutFrameBuffer(*shaderList[ShaderType::BRDF], *meshList.at(MeshType::Cube), captureFBO, captureRBO, brdfLUTTexture);

    // set uniform values
    shaderList[ShaderType::Main]->UseShader();
    shaderList[ShaderType::Main]->SetProjection(projectionMatrix);
    shaderList[ShaderType::Main]->SetIrradianceMap(0);
    shaderList[ShaderType::Main]->SetPrefilterMap(1);
    shaderList[ShaderType::Main]->SetBrdfLUT(2);
    shaderList[ShaderType::Main]->SetRoughnessMap(9);
    shaderList[ShaderType::Main]->SetMetalMap(10);

    shaderList[ShaderType::Environment]->UseShader();
    shaderList[ShaderType::Environment]->SetEnvironmentMap(0);
    shaderList[ShaderType::Environment]->SetProjection(projectionMatrix);


    while (!glfwWindowShouldClose(thisWindow))
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime; 
        lastTime = now;

        glfwPollEvents();
        appObj.inputManager.processInput(*shaderList[ShaderType::Main], thisWindow);
        appObj.camera.keyControl(appObj.inputManager.getsKeys(), deltaTime);
        appObj.camera.mouseControl(appObj.inputManager.getXChange(), appObj.inputManager.getYChange());

        appObj.RenderPass(appObj.camera.calculateViewMatrix(), projectionMatrix, deltaTime, 
                            meshList, textureList, modelList, shaderList, 
                            DirectionalLightList, PointLightList, envCubemap, irradianceMap,
                            prefilterMap, brdfLUTTexture);
        glfwSwapBuffers(thisWindow);
    }

    glfwTerminate();
}


Application::~Application()
{

}

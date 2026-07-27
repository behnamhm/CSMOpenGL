#include "Application.h"



void Application::Init()
{

    window.init(appObj, thisWindow);

}

void Application::Run()
{
    // window init
    Init();
   
    // main shader
    shaderList.push_back(std::make_unique<Shader>());
    shaderList.back()->CreateFromFiles("assets/shaders/vertex.glsl",
                                        "assets/shaders/fragment.glsl");
    // cubemap shader
    shaderList.push_back(std::make_unique<Shader>());
    shaderList.back()->CreateFromFiles("assets/shaders/cubemapVertex.glsl",
                                        "assets/shaders/cubemapFragment.glsl");
    // environment shader
    shaderList.push_back(std::make_unique<Shader>());
    shaderList.back()->CreateFromFiles("assets/shaders/environmentVertex.glsl",
                                       "assets/shaders/environmentFragment.glsl");
    // irradiance shader
    shaderList.push_back(std::make_unique<Shader>());
    shaderList.back()->CreateFromFiles("assets/shaders/cubemapVertex.glsl",
                                       "assets/shaders/irradianceConvolutionFragment.glsl");
    // prefilter shader
    shaderList.push_back(std::make_unique<Shader>());
    shaderList.back()->CreateFromFiles("assets/shaders/cubemapVertex.glsl",
                                        "assets/shaders/prefilterFragment.glsl");
    // brdf shader
    shaderList.push_back(std::make_unique<Shader>());
    shaderList.back()->CreateFromFiles("assets/shaders/BRDFVertex.glsl",
                                        "assets/shaders/BRDFFragment.glsl");

    // meshes
    meshList.push_back(std::make_unique<Mesh>());
    meshList.push_back(std::make_unique<Mesh>());
    meshList.push_back(std::make_unique<Mesh>());
    meshList[0]->DrawFloor();
    meshList[1]->DrawCube();

    // textures
    textureList.push_back(std::make_unique<Texture>());
    *textureList[0] = Texture("assets/textures/checker.png");
    textureList[0]->LoadTexture();

    textureList.push_back(std::make_unique<Texture>());
    *textureList[1] = Texture("assets/textures/Intergalactic Spaceship_metalness.jpg");
    textureList[1]->LoadTexture();

    textureList.push_back(std::make_unique<Texture>());
    *textureList[2] = Texture("assets/textures/checkerMetal.png");
    textureList[2]->LoadTextureA();

    textureList.push_back(std::make_unique<Texture>());
    *textureList[3] = Texture("assets/textures/checkerRoughness.png");
    textureList[3]->LoadTextureA();

    // models
    modelList.push_back(std::make_unique<Model>());
    modelList[0]->LoadModel("assets/Models/Intergalactic_Spaceship-(Wavefront).obj");
   
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
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);
    glm::mat4 camera_view = appObj.camera.calculateViewMatrix();

    // Load HDR
    skybox.LoadHdrEnvironment(hdrTexture);

    // Setup Cube map
    BufferTex.SetupCubeMap(envCubemap);

    // Capture framebuffer
    BufferTex.CaptureFrameBuffer(*shaderList[1], *meshList[1], captureFBO, captureRBO, hdrTexture, envCubemap,
                                  captureProjection, captureViews);

    // Irradiance Map framebuffer
    BufferTex.IrradianceFrameBuffer(*shaderList[3], *meshList[1], captureFBO, captureRBO, irradianceMap, envCubemap,
                                   captureProjection, captureViews);

    // Prefilter framebuffer
    BufferTex.PrefilterFrameBuffer(*shaderList[4], *meshList[1], captureFBO, captureRBO, prefilterMap, envCubemap,
                                   captureProjection, captureViews);

    // brdfLUT framebuffer
    BufferTex.BRDFLutFrameBuffer(*shaderList[5], *meshList[2], captureFBO, captureRBO, brdfLUTTexture);

    // set uniform values
    shaderList[0]->UseShader();
    shaderList[0]->SetProjection(projectionMatrix);
    shaderList[0]->SetIrradianceMap(0);
    shaderList[0]->SetPrefilterMap(1);
    shaderList[0]->SetBrdfLUT(2);
    shaderList[0]->SetRoughnessMap(9);
    shaderList[0]->SetMetalMap(10);

    shaderList[2]->UseShader();
    shaderList[2]->SetEnvironmentMap(0);
    shaderList[2]->SetProjection(projectionMatrix);


    while (!glfwWindowShouldClose(thisWindow))
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime; 
        lastTime = now;

        glfwPollEvents();
        appObj.inputManager.processInput(*shaderList[0], thisWindow);
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

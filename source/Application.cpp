#include "Application.h"



void Application::Init()
{

    window.init(appObj, thisWindow);

}

void Application::Run()
{
    // window and objects initialisation
    Init();

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
                                                                                    -0.5f,-5.0f, -2.5f)));

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
    //appObj.camera = Camera(glm::vec3(0.0f, 6.0f, 9.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 1.0f, 0.5f);
    appObj.camera = Camera(glm::vec3(-3.4f, 2.3f, 2.4f), glm::vec3(0.0f, 1.0f, 0.0f), -50.0f, -20.0f, 1.0f, 0.5f);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), aspect, cameraNearPlane, cameraFarPlane);
    glm::mat4 camera_view = appObj.camera.calculateViewMatrix();

    // Load HDR
    skybox.LoadHdrEnvironment(hdrTexture);

    // Setup Cube map
    Buffer.SetupCubeMap(envCubemap);

    // Capture framebuffer
    Buffer.CaptureFrameBuffer(*shaderList[ShaderType::Cubemap], *meshList.at(MeshType::Cube), captureFBO, captureRBO, hdrTexture, envCubemap,
                                  captureProjection, captureViews);

    // Irradiance Map framebuffer
    Buffer.IrradianceFrameBuffer(*shaderList[ShaderType::Irradiance], *meshList.at(MeshType::Cube), captureFBO, captureRBO, irradianceMap, envCubemap,
                                   captureProjection, captureViews);

    // Prefilter framebuffer
    Buffer.PrefilterFrameBuffer(*shaderList[ShaderType::Prefilter], *meshList.at(MeshType::Cube), captureFBO, captureRBO, prefilterMap, envCubemap,
                                   captureProjection, captureViews);

    // brdfLUT framebuffer
    Buffer.BRDFLutFrameBuffer(*shaderList[ShaderType::BRDF], *meshList.at(MeshType::Cube), captureFBO, captureRBO, brdfLUTTexture);

    // Light depth framebuffer
    Buffer.LightDepthMapBuffer(lightFBO,  lightDepthMaps, depthMapResolution, shadowCascadeLevels);

    // UBO buffer config
    Buffer.ConfigureUBOBuffer(matricesUBO);


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


    while (!glfwWindowShouldClose(thisWindow))
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime; 
        lastTime = now;

        glfwPollEvents();
        appObj.inputManager.processInput(*shaderList[ShaderType::Main], thisWindow);
        appObj.camera.keyControl(appObj.inputManager.getsKeys(), deltaTime);
        appObj.camera.mouseControl(appObj.inputManager.getXChange(), appObj.inputManager.getYChange());


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto lightMatrices = getLightSpaceMatrices();
        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        for (size_t i = 0; i < lightMatrices.size(); ++i)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        appObj.ShadowPass(appObj.camera.calculateViewMatrix(), projectionMatrix, deltaTime,
                        meshList, textureList, modelList, shaderList,
                        DirectionalLightList, PointLightList, envCubemap, irradianceMap,
                        prefilterMap, brdfLUTTexture, lightFBO, depthMapResolution, fb_width, fb_height);


        appObj.RenderPass(appObj.camera.calculateViewMatrix(), projectionMatrix, deltaTime, 
                            meshList, textureList, modelList, shaderList, 
                            DirectionalLightList, PointLightList, envCubemap, irradianceMap,
                            prefilterMap, brdfLUTTexture, shadowCascadeLevels, lightDepthMaps);

        /*
        if (lightMatricesCache.size() != 0)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            debugCascadeShader.use();
            debugCascadeShader.setMat4("projection", projection);
            debugCascadeShader.setMat4("view", view);
            drawCascadeVolumeVisualizers(lightMatricesCache, &debugCascadeShader);
            glDisable(GL_BLEND);
        }

        // render Depth map to quad for visual debugging
        // ---------------------------------------------
        debugDepthQuad.use();
        debugDepthQuad.setInt("layer", debugLayer);
        glActiveTexture(GL_TEXTURE0);                                              
        glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
        if (showQuad)
        {
            renderQuad();
        }
        */

        glfwSwapBuffers(thisWindow);
    }

    glfwTerminate();
}

std::vector<glm::vec4> Application::getFrustumCornersWorldSpace(const glm::mat4& projview)
{
    const auto inv = glm::inverse(projview);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

std::vector<glm::vec4> Application::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    return getFrustumCornersWorldSpace(proj * view);
}

glm::mat4 Application::getLightSpaceMatrix(const float nearPlane, const float farPlane)
{
    const auto proj = glm::perspective(
        glm::radians(90.0f), (float)fb_width / (float)fb_height, nearPlane,
        farPlane);
    const auto corners = getFrustumCornersWorldSpace(proj, appObj.camera.calculateViewMatrix());

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}


std::vector<glm::mat4> Application::getLightSpaceMatrices()
{
    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(cameraNearPlane, shadowCascadeLevels[i]));
        }
        else if (i < shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], cameraFarPlane));
        }
    }
    return ret;
}



#include "Application.h"


bool Application::Init(int width,int height)
{
    return window.init(thisWindow, width, height);
}

void Application::Run()
{
    inputManager.SetCallback(thisWindow);
    scene.Register(thisWindow);

    // Load HDR
    scene.skybox.LoadHdrEnvironment(hdrTexture);

    // Setup Cube map
    Buffer.SetupCubeMap(envCubemap);

    // Capture framebuffer
    Buffer.CaptureFrameBuffer(*scene.shaderList[ShaderType::Cubemap], *scene.meshList.at(MeshType::Cube), 
                                captureFBO, captureRBO, hdrTexture, envCubemap, captureProjection, captureViews);

    // Irradiance Map framebuffer
    Buffer.IrradianceFrameBuffer(*scene.shaderList[ShaderType::Irradiance], *scene.meshList.at(MeshType::Cube), 
                                   captureFBO, captureRBO, irradianceMap, envCubemap, captureProjection, captureViews);

    // Prefilter framebuffer
    Buffer.PrefilterFrameBuffer(*scene.shaderList[ShaderType::Prefilter], *scene.meshList.at(MeshType::Cube), 
                                 captureFBO, captureRBO, prefilterMap, envCubemap, captureProjection, captureViews);

    // brdfLUT framebuffer
    Buffer.BRDFLutFrameBuffer(*scene.shaderList[ShaderType::BRDF], *scene.meshList.at(MeshType::Cube), 
                               captureFBO, captureRBO, brdfLUTTexture);

    // Light depth framebuffer
    Buffer.LightDepthMapBuffer(lightFBO,  lightDepthMaps, depthMapResolution, shadowCascadeLevels);

    // UBO buffer config
    Buffer.ConfigureUBOBuffer(matricesUBO);


    while (!glfwWindowShouldClose(thisWindow))
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime; 
        lastTime = now;

        glfwPollEvents();

        inputManager.processInput(*scene.shaderList[ShaderType::Main], thisWindow, renderer.debugLayer, renderer.showQuad,
                                     renderer.lightMatricesCache, shadowCascadeLevels, scene);

        scene.camera.keyControl(inputManager.getsKeys(), deltaTime);
        scene.camera.mouseControl(inputManager.getXChange(), inputManager.getYChange());

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render passes
        renderer.ShadowPass(scene, deltaTime, lightFBO, depthMapResolution, shadowCascadeLevels,
                          matricesUBO);

        renderer.RenderPass(scene, deltaTime, envCubemap, irradianceMap, prefilterMap,
                         brdfLUTTexture, shadowCascadeLevels, lightDepthMaps);

        glfwSwapBuffers(thisWindow);
    }

    glfwTerminate();
}


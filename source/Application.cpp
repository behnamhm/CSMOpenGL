#include "Application.h"


bool Application::Init(int width,int height)
{
    return window.init(scene, thisWindow, width, height);
}

void Application::Run()
{
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
        scene.inputManager.processInput(*scene.shaderList[ShaderType::Main], thisWindow);
        appObj.processInput(scene, thisWindow, shadowCascadeLevels);
        scene.camera.keyControl(scene.inputManager.getsKeys(), deltaTime);
        scene.camera.mouseControl(scene.inputManager.getXChange(), scene.inputManager.getYChange());

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render passes
        appObj.ShadowPass(scene, deltaTime, lightFBO, depthMapResolution, shadowCascadeLevels, 
                          matricesUBO);

        appObj.RenderPass(scene, deltaTime, envCubemap, irradianceMap, prefilterMap, 
                         brdfLUTTexture, shadowCascadeLevels, lightDepthMaps);

        glfwSwapBuffers(thisWindow);
    }

    glfwTerminate();
}


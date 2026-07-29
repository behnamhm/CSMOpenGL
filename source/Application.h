#pragma once


#include <string>
#include <iostream>

#include "scene/Scene.h"

#include "appObject.h"
#include "components/shader.h"
#include "components/Skybox.h"
#include "components/window.h"
#include "lights/DirectionalLight.h"
#include "lights/PointLight.h"


class Application
{
public:
    void Run();
    bool Init(int width, int height);
    Scene scene;
    appObject appObj;
private:

    std::vector<float> shadowCascadeLevels{ 500.0f / 50.0f,
                                            500.0f / 25.0f,
                                            500.0f / 10.0f,
                                            500.0f / 2.0f };

    glm::mat4 captureProjection;
    std::vector<glm::mat4> captureViews;
    Buffer Buffer;
    GLfloat deltaTime = 0.0f;
    GLfloat lastTime = 0.0f;
    GLFWwindow* thisWindow = nullptr;
    Window window;

    unsigned int depthMapResolution = 4096;
    unsigned int captureFBO, captureRBO, envCubemap, hdrTexture, irradianceMap, 
                prefilterMap, brdfLUTTexture, matricesUBO, lightFBO, lightDepthMaps;
  

};
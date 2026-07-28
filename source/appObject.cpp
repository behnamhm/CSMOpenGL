#include "appObject.h"


void appObject::RenderScene(GLuint uniformModel, GLfloat deltaTime)
{
    // floor
    glm::mat4 floor_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    floor_model *= glm::mat4_cast(glm::angleAxis(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    floor_model = glm::scale(floor_model, glm::vec3(2.0f, 1.0f, 2.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(floor_model));
    textureList.at(TextureType::Checker)->UseTexture(GL_TEXTURE7);
    textureList.at(TextureType::CheckerMetal)->UseTexture(GL_TEXTURE10);
    textureList.at(TextureType::CheckerRoughness)->UseTexture(GL_TEXTURE9);
    shaderList.at(ShaderType::Main)->SetTexture(7);
    meshList.at(MeshType::Floor)->Render();

    // external model
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    textureList.at(TextureType::SpaceshipMetal)->UseTexture(GL_TEXTURE10);
    shaderList.at(ShaderType::Main)->SetTexture(8);
    modelList.at(ModelType::Spaceship)->RenderModel();


}

void appObject::ShadowPass(glm::mat4 camera_view, glm::mat4 projectionMatrix, GLfloat deltaTime,
                            unsigned int& envCubemap, unsigned int& irradianceMap,
                            unsigned int& prefilterMap, unsigned int& brdfLUTTexture, unsigned int& lightFBO, 
                            unsigned int& depthMapResolution,std::vector<float>& shadowCascadeLevels, 
                            unsigned int& matricesUBO, float& cameraNearPlane, float& cameraFarPlane)
{
    const auto lightMatrices = getLightSpaceMatrices(shadowCascadeLevels, cameraNearPlane, cameraNearPlane);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    for (size_t i = 0; i < lightMatrices.size(); ++i)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shaderList.at(ShaderType::ShadowMap)->UseShader();
    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    glViewport(0, 0, depthMapResolution, depthMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);  

    glViewport(0, 0, fb_width, fb_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    uniformModel = shaderList.at(ShaderType::ShadowMap)->GetModelLocation();

    shaderList.at(ShaderType::ShadowMap)->Validate();

    RenderScene(uniformModel, deltaTime);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void appObject::RenderPass(glm::mat4 camera_view, glm::mat4 projectionMatrix, GLfloat deltaTime,
                             unsigned int& envCubemap, unsigned int& irradianceMap,
                            unsigned int& prefilterMap, unsigned int& brdfLUTTexture,
                            std::vector<float> &shadowCascadeLevels, unsigned int& lightDepthMaps)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, 1920, 1080);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // environment cube map
    glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera_view));
    shaderList[ShaderType::Environment]->UseShader();
    shaderList[ShaderType::Environment]->SetView(viewMatrix);
    shaderList[ShaderType::Environment]->SetProjection(projectionMatrix);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    meshList.at(MeshType::Cube)->Render();
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    // set scene data
    shaderList.at(ShaderType::Main)->UseShader();
    uniformModel = shaderList.at(ShaderType::Main)->GetModelLocation();
    uniformEyePosition = shaderList.at(ShaderType::Main)->GetEyePositionLocation();
    uniformProjection = shaderList.at(ShaderType::Main)->GetProjectionLocation();
    uniformView = shaderList.at(ShaderType::Main)->GetViewLocation();
    // bind irradianceMap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
    glm::vec3 camera_pos = camera.getCameraPosition();
    glUniform3f(uniformEyePosition, camera_pos.x, camera_pos.y, camera_pos.z);
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera_view));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    shaderList.at(ShaderType::Main)->SetView(camera_view);
    shaderList.at(ShaderType::Main)->SetDirectionalLight(&*DirectionalLightList[0]);
    shaderList.at(ShaderType::Main)->SetPointLights(PointLightList, 2, 3, 0);
    glm::mat4 lightTransform = DirectionalLightList[0]->CalculateLightTransform();
    shaderList.at(ShaderType::Main)->SetDirectionalLightTransform(&lightTransform);
    shaderList.at(ShaderType::Main)->SetCascadeCount(shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
    {
        shaderList.at(ShaderType::Main)->SetCascadeDistance("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }

    shaderList.at(ShaderType::Main)->Validate();

    // render
    RenderScene(uniformModel, deltaTime);

    // debug
    if (lightMatricesCache.size() != 0)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        shaderList[ShaderType::DebugCascade]->UseShader();
        shaderList[ShaderType::DebugCascade]->SetProjection(projectionMatrix);
        shaderList[ShaderType::DebugCascade]->SetView(camera_view);
        drawCascadeVolumeVisualizers(lightMatricesCache, &*shaderList[ShaderType::DebugCascade]);
        glDisable(GL_BLEND);
    }

    shaderList[ShaderType::DebugDepthQuad]->UseShader();
    shaderList[ShaderType::DebugDepthQuad]->SetLayer(debugLayer);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
    if (showQuad)
    {
        meshList[MeshType::Quad]->DrawQuad();
    }
}

std::vector<glm::mat4> appObject::getLightSpaceMatrices(std::vector<float>& shadowCascadeLevels, 
                                                    float& cameraNearPlane, float& cameraFarPlane)
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

glm::mat4 appObject::getLightSpaceMatrix(const float nearPlane, const float farPlane)
{
    const auto proj = glm::perspective(
        glm::radians(90.0f), (float)fb_width / (float)fb_height, nearPlane,
        farPlane);
    const auto corners = getFrustumCornersWorldSpace(proj, camera.calculateViewMatrix());

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
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

std::vector<glm::vec4> appObject::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    return getFrustumCornersWorldSpace(proj * view);
}


std::vector<glm::vec4> appObject::getFrustumCornersWorldSpace(const glm::mat4& projview)
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

void appObject::drawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader)
{
    visualizerVAOs.resize(8);
    visualizerEBOs.resize(8);
    visualizerVBOs.resize(8);

    const GLuint indices[] = {
        0, 2, 3,
        0, 3, 1,
        4, 6, 2,
        4, 2, 0,
        5, 7, 6,
        5, 6, 4,
        1, 3, 7,
        1, 7, 5,
        6, 7, 3,
        6, 3, 2,
        1, 5, 4,
        0, 1, 4
    };

    const glm::vec4 colors[] = {
        {1.0, 0.0, 0.0, 0.5f},
        {0.0, 1.0, 0.0, 0.5f},
        {0.0, 0.0, 1.0, 0.5f},
    };

    for (int i = 0; i < lightMatrices.size(); ++i)
    {
        const auto corners = getFrustumCornersWorldSpace(lightMatrices[i]);
        std::vector<glm::vec3> vec3s;
        for (const auto& v : corners)
        {
            vec3s.push_back(glm::vec3(v));
        }

        glGenVertexArrays(1, &visualizerVAOs[i]);
        glGenBuffers(1, &visualizerVBOs[i]);
        glGenBuffers(1, &visualizerEBOs[i]);

        glBindVertexArray(visualizerVAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, visualizerVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizerEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glBindVertexArray(visualizerVAOs[i]);
        shader->SetColors(colors[i % 3]);
        glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);

        glDeleteBuffers(1, &visualizerVBOs[i]);
        glDeleteBuffers(1, &visualizerEBOs[i]);
        glDeleteVertexArrays(1, &visualizerVAOs[i]);

        glBindVertexArray(0);
    }

    visualizerVAOs.clear();
    visualizerEBOs.clear();
    visualizerVBOs.clear();
}


void appObject::processInput(GLFWwindow* window, std::vector<float>& shadowCascadeLevels)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    static int fPress = GLFW_RELEASE;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && fPress == GLFW_PRESS)
    {
        showQuad = !showQuad;
    }
    fPress = glfwGetKey(window, GLFW_KEY_F);

    static int plusPress = GLFW_RELEASE;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE && plusPress == GLFW_PRESS)
    {
        debugLayer++;
        if (debugLayer > shadowCascadeLevels.size())
        {
            debugLayer = 0;
        }
    }
    plusPress = glfwGetKey(window, GLFW_KEY_N);

    static int cPress = GLFW_RELEASE;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && cPress == GLFW_PRESS)
    {

        lightMatricesCache = getLightSpaceMatrices(shadowCascadeLevels,
            cameraNear, cameraFar);
    }
    cPress = glfwGetKey(window, GLFW_KEY_C);
}


#include "Renderer.h"
#include "scene/Scene.h"



void Renderer::RenderScene(Scene& scene, Shader* shader, GLuint uniformModel, GLfloat deltaTime)
{
    // floor
    glm::mat4 floor_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    floor_model *= glm::mat4_cast(glm::angleAxis(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    floor_model = glm::scale(floor_model, glm::vec3(2.0f, 1.0f, 2.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(floor_model));
    scene.textureList.at(TextureType::Checker)->UseTexture(GL_TEXTURE7);
    scene.textureList.at(TextureType::CheckerMetal)->UseTexture(GL_TEXTURE10);
    scene.textureList.at(TextureType::CheckerRoughness)->UseTexture(GL_TEXTURE9);
    shader->SetTexture(7);
    scene.meshList.at(MeshType::Floor)->Render();

    // external model
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    scene.textureList.at(TextureType::SpaceshipMetal)->UseTexture(GL_TEXTURE10);
    shader->SetTexture(8);
    scene.modelList.at(ModelType::Spaceship)->RenderModel();

}

void Renderer::ShadowPass(Scene& scene, GLfloat deltaTime, unsigned int& lightFBO,
                            unsigned int& depthMapResolution,std::vector<float>& shadowCascadeLevels, 
                            unsigned int& matricesUBO)
{
    const auto lightMatrices = shadowSystem.getLightSpaceMatrices(scene, shadowCascadeLevels, scene.cameraNearPlane, scene.cameraNearPlane);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    for (size_t i = 0; i < lightMatrices.size(); ++i)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    scene.shaderList.at(ShaderType::ShadowMap)->UseShader();
    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    glViewport(0, 0, depthMapResolution, depthMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);  

    glViewport(0, 0, shadowSystem.fb_width, shadowSystem.fb_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    uniformModel = scene.shaderList.at(ShaderType::ShadowMap)->GetModelLocation();

    scene.shaderList.at(ShaderType::ShadowMap)->Validate();

    RenderScene(scene, &*scene.shaderList.at(ShaderType::Main), uniformModel, deltaTime);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::RenderPass(Scene& scene, GLfloat deltaTime,
                             unsigned int& envCubemap, unsigned int& irradianceMap,
                            unsigned int& prefilterMap, unsigned int& brdfLUTTexture,
                            std::vector<float> &shadowCascadeLevels, unsigned int& lightDepthMaps)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, 1920, 1080);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // environment cube map
    glm::mat4 viewMatrix = glm::mat4(glm::mat3(scene.camera.calculateViewMatrix()));
    scene.shaderList[ShaderType::Environment]->UseShader();
    scene.shaderList[ShaderType::Environment]->setUniform("view", viewMatrix);
    scene.shaderList[ShaderType::Environment]->setUniform("projection", scene.projectionMatrix);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    scene.meshList.at(MeshType::Cube)->Render();
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    // set scene data
    scene.shaderList.at(ShaderType::Main)->UseShader();
    uniformModel = scene.shaderList.at(ShaderType::Main)->GetModelLocation();
    uniformEyePosition = scene.shaderList.at(ShaderType::Main)->GetEyePositionLocation();
    uniformProjection = scene.shaderList.at(ShaderType::Main)->GetProjectionLocation();
    uniformView = scene.shaderList.at(ShaderType::Main)->GetViewLocation();
    // bind irradianceMap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
    glm::vec3 camera_pos = scene.camera.getCameraPosition();
    glUniform3f(uniformEyePosition, camera_pos.x, camera_pos.y, camera_pos.z);
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(scene.camera.calculateViewMatrix()));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(scene.projectionMatrix));

    scene.shaderList.at(ShaderType::Main)->setUniform("view", scene.camera.calculateViewMatrix());
    scene.shaderList.at(ShaderType::Main)->SetDirectionalLight(&*scene.DirectionalLightList[0]);
    scene.shaderList.at(ShaderType::Main)->SetPointLights(scene.PointLightList, 2, 3, 0);
    glm::mat4 lightTransform = scene.DirectionalLightList[0]->CalculateLightTransform();
    scene.shaderList.at(ShaderType::Main)->setUniform("directionalLightTransform", &lightTransform);
    int shadowLevelCount = shadowCascadeLevels.size();
    scene.shaderList.at(ShaderType::Main)->setUniform("cascadeCount", shadowLevelCount);
    for (size_t i = 0; i < shadowLevelCount; ++i)
    {
        scene.shaderList.at(ShaderType::Main)->setUniform("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }

    scene.shaderList.at(ShaderType::Main)->Validate();

    // render
    RenderScene(scene, &*scene.shaderList.at(ShaderType::Main), uniformModel, deltaTime);

    // debug

    if (lightMatricesCache.size() != 0)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_STENCIL_TEST);

        glClear(GL_STENCIL_BUFFER_BIT);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f);

        glDepthMask(GL_FALSE);
        scene.shaderList[ShaderType::DebugCascade]->UseShader();
        scene.shaderList[ShaderType::DebugCascade]->setUniform("projection", scene.projectionMatrix);
        scene.shaderList[ShaderType::DebugCascade]->setUniform("view", scene.camera.calculateViewMatrix());

        int shadowLevelCount = shadowCascadeLevels.size();
        scene.shaderList[ShaderType::DebugCascade]->setUniform("cascadeCount",
            shadowLevelCount);

        for (int i = 0; i < shadowCascadeLevels.size(); ++i)
        {
            scene.shaderList[ShaderType::DebugCascade]->setUniform(
                "cascadePlaneDistances[" + std::to_string(i) + "]",
                shadowCascadeLevels[i]);
        }
        GLuint debugUniformModel =
            scene.shaderList[ShaderType::DebugCascade]->GetModelLocation();

        RenderScene(scene, &*scene.shaderList.at(ShaderType::DebugCascade), debugUniformModel, deltaTime);
        glDepthMask(GL_TRUE);

        glDisable(GL_POLYGON_OFFSET_FILL);

        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);

    }

    scene.shaderList[ShaderType::DebugDepthQuad]->setUniform("layer", debugLayer);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
    if (showQuad)
    {
        scene.meshList[MeshType::Quad]->DrawQuad();
    }
}



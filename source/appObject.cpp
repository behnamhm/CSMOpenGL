#include "appObject.h"


void appObject::RenderScene(GLuint uniformModel, GLfloat deltaTime,
                            std::unordered_map<MeshType, std::unique_ptr<Mesh>>& meshList,
                            std::unordered_map<TextureType, std::unique_ptr<Texture>>& textureList,
                            std::unordered_map<ModelType, std::unique_ptr<Model>>& modelList, 
                            std::unordered_map<ShaderType, std::unique_ptr<Shader>>& shaderList)
{
    // floor
    glm::mat4 floor_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    floor_model *= glm::mat4_cast(glm::angleAxis(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    floor_model = glm::scale(floor_model,
        glm::vec3(2.0f, 1.0f, 2.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(floor_model));

    meshList.at(MeshType::Floor)->Render();

    // external model
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    textureList.at(TextureType::SpaceshipMetal)->UseTexture(GL_TEXTURE10);
    shaderList.at(ShaderType::Main)->SetTexture(8);
    
    modelList.at(ModelType::Spaceship)->RenderModel();


}

void appObject::ShadowPass(glm::mat4 camera_view, glm::mat4 projectionMatrix, GLfloat deltaTime,
                            std::unordered_map<MeshType, std::unique_ptr<Mesh>>& meshList,
                            std::unordered_map<TextureType, std::unique_ptr<Texture>>& textureList,
                            std::unordered_map<ModelType, std::unique_ptr<Model>>& modelList,
                            std::unordered_map<ShaderType, std::unique_ptr<Shader>>& shaderList,
                            std::vector<std::unique_ptr<DirectionalLight>>& DirectionalLightList,
                            std::vector<std::unique_ptr<PointLight>>& PointLightList,
                            unsigned int& envCubemap, unsigned int& irradianceMap,
                            unsigned int& prefilterMap, unsigned int& brdfLUTTexture, unsigned int& lightFBO, 
                            unsigned int& depthMapResolution, int& fb_width, int& fb_height)
{
    shaderList.at(ShaderType::ShadowMap)->UseShader();
    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    glViewport(0, 0, depthMapResolution, depthMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);  // peter panning



    // reset viewport
    glViewport(0, 0, fb_width, fb_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    uniformModel = shaderList.at(ShaderType::ShadowMap)->GetModelLocation();


    shaderList.at(ShaderType::ShadowMap)->Validate();

    RenderScene(uniformModel, deltaTime, meshList, textureList, modelList, shaderList);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void appObject::RenderPass(glm::mat4 camera_view, glm::mat4 projectionMatrix, GLfloat deltaTime,
                            std::unordered_map<MeshType, std::unique_ptr<Mesh>>& meshList,
                            std::unordered_map<TextureType, std::unique_ptr<Texture>>& textureList,
                            std::unordered_map<ModelType, std::unique_ptr<Model>>& modelList,
                            std::unordered_map<ShaderType, std::unique_ptr<Shader>>& shaderList,
                            std::vector<std::unique_ptr<DirectionalLight>>& DirectionalLightList,
                            std::vector<std::unique_ptr<PointLight>>& PointLightList,
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

    shaderList[ShaderType::Main]->SetView(camera_view);

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


    textureList.at(TextureType::Checker)->UseTexture(GL_TEXTURE7);
    textureList.at(TextureType::CheckerMetal)->UseTexture(GL_TEXTURE10);
    textureList.at(TextureType::CheckerRoughness)->UseTexture(GL_TEXTURE9);
    shaderList.at(ShaderType::Main)->SetTexture(7);

    // render
    RenderScene(uniformModel, deltaTime, meshList, textureList, modelList, shaderList);
}

appObject::~appObject()
{

}

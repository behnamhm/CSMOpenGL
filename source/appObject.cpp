#include "appObject.h"


void appObject::RenderScene(GLuint uniformModel, GLfloat deltaTime,
                            std::vector<std::unique_ptr<Mesh>>& meshList,
                            std::vector<std::unique_ptr<Texture>>& textureList,
                            std::vector<std::unique_ptr<Model>>& modelList,
                            std::vector<std::unique_ptr<Shader>>& shaderList)
{
    // floor
    glm::mat4 floor_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    floor_model *= glm::mat4_cast(glm::angleAxis(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    floor_model = glm::scale(floor_model,
        glm::vec3(2.0f, 1.0f, 2.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(floor_model));
    textureList[0]->UseTexture(GL_TEXTURE7);
    textureList[2]->UseTexture(GL_TEXTURE10);
    textureList[3]->UseTexture(GL_TEXTURE9);      
    shaderList[0]->SetTexture(7);
    meshList[0]->Draw();

    // external model
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    textureList[1]->UseTexture(GL_TEXTURE10);
    shaderList[0]->SetTexture(8);
    
    modelList[0]->RenderModel();


}

void appObject::RenderPass(glm::mat4 camera_view, glm::mat4 projectionMatrix, GLfloat deltaTime,
                            std::vector<std::unique_ptr<Mesh>>& meshList,
                            std::vector<std::unique_ptr<Texture>>& textureList,
                            std::vector<std::unique_ptr<Model>>& modelList,
                            std::vector<std::unique_ptr<Shader>>& shaderList,
                            std::vector<std::unique_ptr<DirectionalLight>>& DirectionalLightList,
                            std::vector<std::unique_ptr<PointLight>>& PointLightList,
                            unsigned int& envCubemap, unsigned int& irradianceMap,
                            unsigned int& prefilterMap, unsigned int& brdfLUTTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, 1920, 1080);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // environment cube map
    glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera_view));
    shaderList[2]->UseShader();
    shaderList[2]->SetView(viewMatrix);
    shaderList[2]->SetProjection(projectionMatrix);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    meshList[1]->Draw();
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    // set scene data
    shaderList[0]->UseShader();
    uniformModel = shaderList[0]->GetModelLocation();
    uniformEyePosition = shaderList[0]->GetEyePositionLocation();
    uniformProjection = shaderList[0]->GetProjectionLocation();
    uniformView = shaderList[0]->GetViewLocation();
    // bind irradianceMap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

    glm::vec3 camera_pos = camera.getCameraPosition();
    glUniform3f(uniformEyePosition, camera_pos.x, camera_pos.y, camera_pos.z);
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera_view));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    shaderList[0]->SetDirectionalLight(&*DirectionalLightList[0]);
    shaderList[0]->SetPointLights(PointLightList, 2, 3, 0);
    glm::mat4 lightTransform = DirectionalLightList[0]->CalculateLightTransform();
    shaderList[0]->SetDirectionalLightTransform(&lightTransform);

    shaderList[0]->Validate();

    // render
    RenderScene(uniformModel, deltaTime, meshList, textureList, modelList, shaderList);
}

appObject::~appObject()
{

}

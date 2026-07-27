#include "appObject.h"


void appObject::RenderScene(GLuint uniformModel, GLfloat deltaTime,
                            std::unordered_map<MeshType, std::unique_ptr<Mesh>>& meshList,
                            std::vector<std::unique_ptr<Texture>>& textureList,
                            std::vector<std::unique_ptr<Model>>& modelList,
                            std::unordered_map<ShaderType, std::unique_ptr<Shader>>& shaderList)
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
    shaderList.at(ShaderType::Main)->SetTexture(7);
    meshList.at(MeshType::Floor)->Draw();

    // external model
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    textureList[1]->UseTexture(GL_TEXTURE10);
    shaderList.at(ShaderType::Main)->SetTexture(8);
    
    modelList[0]->RenderModel();


}

void appObject::RenderPass(glm::mat4 camera_view, glm::mat4 projectionMatrix, GLfloat deltaTime,
                            std::unordered_map<MeshType, std::unique_ptr<Mesh>>& meshList,
                            std::vector<std::unique_ptr<Texture>>& textureList,
                            std::vector<std::unique_ptr<Model>>& modelList,
                            std::unordered_map<ShaderType, std::unique_ptr<Shader>>& shaderList,
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
    shaderList[ShaderType::Environment]->UseShader();
    shaderList[ShaderType::Environment]->SetView(viewMatrix);
    shaderList[ShaderType::Environment]->SetProjection(projectionMatrix);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    meshList.at(MeshType::Cube)->Draw();
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

    glm::vec3 camera_pos = camera.getCameraPosition();
    glUniform3f(uniformEyePosition, camera_pos.x, camera_pos.y, camera_pos.z);
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera_view));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    shaderList.at(ShaderType::Main)->SetDirectionalLight(&*DirectionalLightList[0]);
    shaderList.at(ShaderType::Main)->SetPointLights(PointLightList, 2, 3, 0);
    glm::mat4 lightTransform = DirectionalLightList[0]->CalculateLightTransform();
    shaderList.at(ShaderType::Main)->SetDirectionalLightTransform(&lightTransform);

    shaderList.at(ShaderType::Main)->Validate();

    // render
    RenderScene(uniformModel, deltaTime, meshList, textureList, modelList, shaderList);
}

appObject::~appObject()
{

}

#include "inputManager.h"
#include <GLFW\glfw3.h>



InputManager::InputManager() 

{  
    for (size_t i = 0; i < 1024; i++)
    {
        keys[i] = 0;
    }

    xChange = 0.0f;
    yChange = 0.0f;
    offset = glm::vec2{ 0.0f, 0.0f };
}


void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{


    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            offset.y += 0.01f;
            break;
        case GLFW_KEY_S:
            offset.y -= 0.01f;
            break;
        case GLFW_KEY_D:
            offset.x += 0.01f;
            break;
        case GLFW_KEY_A:
            offset.x -= 0.01f;
            break;
        }
    }
}

void InputManager::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
    InputManager* theWindow = static_cast<InputManager*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            theWindow->keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            theWindow->keys[key] = false;
        }
    }
}

void InputManager::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
    InputManager* theWindow = static_cast<InputManager*>(glfwGetWindowUserPointer(window));

    if (theWindow->mouseFirstMoved)
    {
        theWindow->lastX = xPos;
        theWindow->lastY = yPos;
        theWindow->mouseFirstMoved = false;
    }

    theWindow->xChange = xPos - theWindow->lastX;
    theWindow->yChange = theWindow->lastY - yPos;

    theWindow->lastX = xPos;
    theWindow->lastY = yPos;
}

void InputManager::SetCallback(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, handleKeys);
    glfwSetCursorPosCallback(window, handleMouse);


}

GLfloat InputManager::getXChange()
{
    GLfloat theChange = xChange;
    xChange = 0.0f;
    return theChange;
}

GLfloat InputManager::getYChange()
{
    GLfloat theChange = yChange;
    yChange = 0.0f;
    return theChange;
}

void InputManager::processInput(Shader& shader, GLFWwindow* window, int& debugLayer, bool& showQuad,
                            std::vector<glm::mat4>& lightMatricesCache,
                            std::vector<float>& shadowCascadeLevels, Scene& scene)
{
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (metallicVal > 0.0f){
            metallicVal -= 0.01f;
        }
      
        else
        {
            metallicVal = 0.0f;
        }


    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        metallicVal += 0.01f;

    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if (roughnessVal > 0.0f) {
            roughnessVal -= 0.01f;
        }

        else
        {
            roughnessVal = 0.00f;
        }


    }
    else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        roughnessVal += 0.01f;

    }

    
    shader.setUniform("metallic", std::min(metallicVal, 1.0f));
    shader.setUniform("roughness", std::min(roughnessVal, 1.0f));

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
    float cameraNear = 0.1f;
    float cameraFar = 500.0f;
    ShadowSystem shadowSystem;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && cPress == GLFW_PRESS)

    {
        if (lightMatricesCache.size() != 0)
        {
            lightMatricesCache.clear();
        }
        else
        {
        lightMatricesCache = shadowSystem.getLightSpaceMatrices(scene, shadowCascadeLevels,
            cameraNear, cameraFar);

        }
    }
    cPress = glfwGetKey(window, GLFW_KEY_C);

}



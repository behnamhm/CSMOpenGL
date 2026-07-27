#include "inputManager.h"
#include <GLFW\glfw3.h>

glm::vec2 InputManager::offset = glm::vec2(0.0f, 0.0f);


InputManager::InputManager() 

{  
    for (size_t i = 0; i < 1024; i++)
    {
        keys[i] = 0;
    }

    xChange = 0.0f;
    yChange = 0.0f;
}


void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            InputManager::offset.y += 0.01f;
            break;
        case GLFW_KEY_S:
            InputManager::offset.y -= 0.01f;
            break;
        case GLFW_KEY_D:
            InputManager::offset.x += 0.01f;
            break;
        case GLFW_KEY_A:
            InputManager::offset.x -= 0.01f;
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

void InputManager::processInput(Shader &shader, GLFWwindow* window)
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
    shader.SetMetallic(std::min(metallicVal, 1.0f));
    shader.SetRoughness(std::min(roughnessVal, 1.0f));
    

}



InputManager::~InputManager()
{

}
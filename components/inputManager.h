#pragma once
#include <array>
#include "shader.h"
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>


class InputManager
{
public:
    InputManager();
    void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
    static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
    static void handleMouse(GLFWwindow* window, double xPos, double yPos);
    GLfloat getXChange();
    GLfloat getYChange();

    void processInput(Shader& shader, GLFWwindow* window, int &debugLayer, bool& showQuad, 
                        std::vector<glm::mat4> &lightMatricesCache,
                        std::vector<float> &shadowCascadeLevels, Scene& scene);
    void SetCallback(GLFWwindow* window);

    glm::vec2 offset;
    bool* getsKeys() { return keys; }
    float metallicVal = 0.0f;
    float roughnessVal = 0.0f;

private:
    bool keys[1024];
    GLfloat lastX;
    GLfloat lastY;
    GLfloat xChange;
    GLfloat yChange;
    bool mouseFirstMoved;


};

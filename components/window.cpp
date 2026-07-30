#include "window.h"

Window::Window()
{

}

bool Window::init(GLFWwindow*& thisWindow, int width, int height)
{
    if (!glfwInit())
    {
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);




    thisWindow = glfwCreateWindow(width, height, "IBL App", nullptr, nullptr);

    if (thisWindow == nullptr)
    {
        std::cout << "Error Creating window" << std::endl;
        glfwTerminate();
        return -1;
    }




    //glfwSetWindowPos(window, 2000, 150);
    glfwMakeContextCurrent(thisWindow);

    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        return -1;
    }
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    return true;
}

Window::~Window()
{

}
#include "window.h"

Window::Window()
{

}

bool Window::init(appObject &appObj, GLFWwindow*& thisWindow)
{
    if (!glfwInit())
    {
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);




    thisWindow = glfwCreateWindow(1920, 1080, "IBL App", nullptr, nullptr);

    if (thisWindow == nullptr)
    {
        std::cout << "Error Creating window" << std::endl;
        glfwTerminate();
        return -1;
    }

    appObj.inputManager.SetCallback(thisWindow);


    //glfwSetWindowPos(window, 2000, 150);
    glfwMakeContextCurrent(thisWindow);

    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    return true;
}

Window::~Window()
{

}
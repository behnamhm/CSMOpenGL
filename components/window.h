#pragma once


#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "scene/Scene.h"

class Window
{
public:
	Window();

	bool init(GLFWwindow*& thisWindow, int width, int height);

	~Window();


};

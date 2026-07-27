#pragma once


#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "source/appObject.h"

class Window
{
public:
	Window();

	bool init( appObject& appObj, GLFWwindow*& thisWindow);

	~Window();


};

#pragma once

#include <glfw3.h>
#include "Globals.h"



class Window {
public:
	Window() {

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		if (_window == nullptr)
		{
			_window = glfwCreateWindow(1600,1200 , "CS560 Animation: Siddharth Gupta", NULL, NULL);
			glfwMakeContextCurrent(_window);
			
		}


		
	}

	inline GLFWwindow* GetWindow()const { return _window; }

private:
	GLFWwindow* _window;


};


#pragma once
#include "Camera.h"
#include "Window.h"
#include "Callbacks.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <unordered_map>

//IMGUI
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw_gl3.h"
#include "Imgui/ImGuizmo.h"

#include <iostream>

//#include "Skeleton.h"

// settings
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

// Camera
extern Camera camera;
extern float lastX;
extern float lastY ;
extern bool firstMouse;

//Window
extern Window window;

//Callback 
extern Callbacks cb;

//Skeleton
//extern Skeleton skeleton;

// timing
extern float deltaTime;
extern float lastFrame ;


//MACROS
#define GET_WINDOW(window)\
window.GetWindow()

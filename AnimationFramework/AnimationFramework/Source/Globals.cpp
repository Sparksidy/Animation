#include "Globals.h"

//Screen Properties
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

//Camera
Camera camera(glm::vec3(0.0f, 10.0f, 60.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Window window;
Callbacks cb;
//Skeleton skeleton;

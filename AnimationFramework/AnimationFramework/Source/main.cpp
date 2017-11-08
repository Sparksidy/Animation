
#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "freeglut.h"
#include <iostream>
#include <Windows.h>

//IMGUI
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw_gl3.h"

//MY CLASSES
#include "Shader.h"
#include "Model.h"
#include "SkinnedMesh.h"
#include "util.h"
#include "Camera.h"
#include "Globals.h"
#include "Plane.h"
#include "Callbacks.h"
#include "Skeleton.h"
#include "CatmullRomSpline.h"

//FUNCTION PROTOTYPES
void mouse_callback_debug(GLFWwindow* window, double xpos, double ypos);
void PrintControlPoints(CatMullRomSpline& spline);


int main()
{
	if (!glewInit())
		cout << "Unable to inititalize GLEW" << endl;

	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);

	//Shaders
	Shader skeletalAnimationShader("Source//Shaders//skeletalAnimation.vert", "Source//Shaders//skeletalAnimation.frag");		//Shader for skinned mesh
	Shader pointShader("Source//Shaders//skeleton.vert", "Source//Shaders//skeleton.frag");										//Shader for bones
	Shader simpleShader("Source//Shaders//simpleShader.vert", "Source//Shaders//simpleShader.frag");							//Shader for plane
	Shader curveShader("Source//Shaders//curve.vert", "Source//Shaders//curve.frag");

	//Stuff to Render
	Plane plane;									//Floor
	SkinnedMesh doom;								//Model
	Skeleton skeleton;								//Skeleton

	CatMullRomSpline spline;						//Curve
	spline.DesignCurve();							//Design the curve
	PrintControlPoints(spline);						//Print on Console
	spline.FillBuffers();

	doom.LoadMesh("Resources//Tiny//tiny_4anim.x");		
	doom.SendBonesLocationToShader(skeletalAnimationShader);

	
	ImGui_ImplGlfwGL3_Init(GET_WINDOW(window), true);
	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	float startTime = GetTickCount();
	while (!glfwWindowShouldClose(GET_WINDOW(window)))
	{
		ImGui_ImplGlfwGL3_NewFrame();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		float RunningTime = (float)((double)GetTickCount() - (double)startTime) / 1000.0f;

		
		cb.processInput(GET_WINDOW(window));
	
		
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		
		//spline.DrawCurve(curveShader);

		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	
		vector<Matrix4f> Transforms;
		vector<Matrix4f> BonePosition;
		doom.SetMVP(skeletalAnimationShader);
		doom.UpdateBoneTransforms(Transforms, BonePosition, RunningTime);

	
		spline.UpdateMVP(curveShader);		//Fill the buffer


		int status = glfwGetKey(GET_WINDOW(window), GLFW_KEY_C);
		if (status == GLFW_PRESS)
		{
			skeleton.UpdateSkeletonBuffers(pointShader, BonePosition);
			skeleton.DrawSkeleton(pointShader);
		}
		else if (status == GLFW_RELEASE)
		{
			//plane.Render(simpleShader);
			//doom.Render(skeletalAnimationShader);
			

			spline.DrawCurve(curveShader);
		}
	/*	int debugStatus = glfwGetKey(GET_WINDOW(window), GLFW_KEY_Q);
		if (debugStatus == GLFW_PRESS)
		{
			ImGui::Render();
			glfwSetCursorPosCallback(GET_WINDOW(window), NULL);

		}
		else if (debugStatus == GLFW_RELEASE)
		{
			glfwSetCursorPosCallback(GET_WINDOW(window), mouse_callback_debug);
		
		}*/

		glfwSwapBuffers(GET_WINDOW(window));
		glfwPollEvents();
	}

	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();
	return 0;
}


void mouse_callback_debug(GLFWwindow * window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void PrintControlPoints(CatMullRomSpline& spline)
{
	/*for (int i = 0; i < spline.interpolatedPoints.size(); i++)
	{
		std::cout << spline.interpolatedPoints[i].x <<" "<< spline.interpolatedPoints[i].y<<" "<< spline.interpolatedPoints[i].z<< std::endl;
	}*/
}
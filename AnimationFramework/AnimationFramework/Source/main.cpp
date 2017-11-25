
#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "freeglut.h"
#include <Windows.h>



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
#include "TransformEditor.h"
#include "CyclicCoordinateDescend.h"

//FUNCTION PROTOTYPES
void mouse_callback_debug(GLFWwindow* window, double xpos, double ypos);



int main()
{
	if (!glewInit())
		cout << "Unable to inititalize GLEW" << endl;

	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);

	//Shaders
	Shader skeletalAnimationShader("Source/Shaders/skeletalAnimation.vert", "Source/Shaders/skeletalAnimation.frag");		//Shader for skinned mesh
	Shader pointShader("Source//Shaders//skeleton.vert", "Source//Shaders//skeleton.frag");										//Shader for bones
	Shader simpleShader("Source//Shaders//simpleShader.vert", "Source//Shaders//simpleShader.frag");							//Shader for plane
	Shader curveShader("Source//Shaders//curve.vert", "Source//Shaders//curve.frag");

	//Stuff to Render
	Plane plane;									//Floor
	SkinnedMesh doom;								//Model
	Skeleton skeleton;								//Skeleton

	//Curve
	CatMullRomSpline spline;						//Curve
	spline.DesignCurve();							//Design the curve
	spline.FillBuffers();							//Fill the buffers with control points
	spline.DesignTable();							//Forward Differencing Table


	doom.LoadMesh("Resources/Tiny/tiny_4anim.x");
	doom.SendBonesLocationToShader(skeletalAnimationShader);

	
	
	//DEBUG
	TransformEditor transformEditor;
	glm::mat4 model;
	ImGui_ImplGlfwGL3_Init(GET_WINDOW(window), true);
	bool show_test_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	bool drawskeleton = false;

	//IK
	glm::vec3 targetPoint = { 0,10,10 };
	CyclicCoordinateDescend ccd;
	


	float startTime = GetTickCount();
	while (!glfwWindowShouldClose(GET_WINDOW(window)))
	{
		ImGui_ImplGlfwGL3_NewFrame();
		ImGuizmo::BeginFrame();

		
		transformEditor.Update(model);
		
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		float RunningTime = (float)((double)GetTickCount() - (double)startTime)/ 1000.0f;

		
		cb.processInput(GET_WINDOW(window));
	
		
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
	
		vector<Matrix4f> Transforms;
		vector<Matrix4f> BonePosition;
		doom.SetMVP(skeletalAnimationShader);
		float a = spline.GetVelocity() / spline.GetSpeed();
		doom.UpdateBoneTransforms(Transforms, BonePosition, RunningTime, a);

		spline.UpdateMVP(curveShader);

		ccd.ComputeCCDLink(targetPoint, doom);


		int status = glfwGetKey(GET_WINDOW(window), GLFW_KEY_C);
		if (drawskeleton)
		{
			//spline.Update(RunningTime, doom,deltaTime);
			plane.Render(simpleShader);
			skeleton.UpdateSkeletonBuffers(pointShader, BonePosition, doom);
			skeleton.DrawSkeleton(pointShader);
			//spline.DrawCurve(curveShader);
		}
		else
		{
			//spline.Update(RunningTime, doom, deltaTime);
			Vector3f vec = { 0,0,0 };
			doom.SetModelsPosition(vec);
			plane.Render(simpleShader);
			doom.Render(skeletalAnimationShader);
			//spline.DrawCurve(curveShader);
		}
		

		ImGui::Checkbox("DrawSkeleton", &drawskeleton);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		
		ImGui::Render();
		
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


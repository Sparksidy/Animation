#pragma once

//IMGUI
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw_gl3.h"
#include "Imgui/ImGuizmo.h"

//GLM
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <glew.h>

//MY CLASSES
#include "Globals.h"
#include "Shader.h"

class TransformEditor {

public:
	TransformEditor();
	~TransformEditor();

	void Update(glm::mat4& model);
	void Reset();

private:
	ImGuizmo::OPERATION		_currentOperation;
	ImGuizmo::MODE			_currentMode;
	

};

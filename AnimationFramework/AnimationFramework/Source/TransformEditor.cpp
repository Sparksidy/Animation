#include "TransformEditor.h"


TransformEditor::TransformEditor()
{
	_currentOperation	=		ImGuizmo::TRANSLATE;
	_currentMode		=		ImGuizmo::LOCAL;

}

TransformEditor::~TransformEditor()
{
}

void TransformEditor::Update(glm::mat4 & matrix)
{
	if (ImGui::RadioButton("Translate", _currentOperation == ImGuizmo::TRANSLATE))
		_currentOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", _currentOperation == ImGuizmo::ROTATE))
		_currentOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", _currentOperation == ImGuizmo::SCALE))
		_currentOperation = ImGuizmo::SCALE;


	glm::mat4 const& view_matrix = camera.GetViewMatrix();
	glm::mat4 const& projection_matrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	glm::mat4 modelMatrix = matrix;

	
	float translation[3];
	float rotation[3];
	float scale[3];
	ImGuizmo::DecomposeMatrixToComponents(&matrix[0][0], translation, rotation, scale);
	ImGui::InputFloat3("Tr", translation, 3);
	ImGui::InputFloat3("Rt", rotation, 3);
	ImGui::InputFloat3("Sc", scale, 3);

	ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, &matrix[0][0]);

	ImGuiIO & io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(&view_matrix[0][0], &projection_matrix[0][0], _currentOperation, _currentMode, &modelMatrix[0][0]);

	
}

void TransformEditor::Reset()
{
	_currentOperation = ImGuizmo::TRANSLATE;
	_currentMode = ImGuizmo::LOCAL;
}

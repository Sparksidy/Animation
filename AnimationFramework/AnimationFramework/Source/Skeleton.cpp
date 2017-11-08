#include <vector>
#include <Windows.h>

#include "Skeleton.h"
#include "Globals.h"
#include "math_3d.h"

Skeleton::Skeleton()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(0);
}

void Skeleton::UpdateSkeletonBuffers(Shader& shader, vector<Matrix4f>& BonePosition)
{
	//Calculate the MVP of the bones
	int num_Bones = BonePosition.size();
	glm::vec4* jointPosition = new glm::vec4[num_Bones];
	std::vector<Vector4f> BonePositionsMeshSpace;
	glm::mat4 model, projection, view;
	//pointModel = glm::translate(pointModel, glm::vec3(0.0f, -1.75f, 0.0f));
	//pointModel = glm::rotate(pointModel, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
	//pointModel = glm::scale(pointModel, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
	model = glm::translate(model, glm::vec3(0.0f, 1.0f, -10.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
	model = glm::rotate(model, glm::radians(180.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
	model = glm::rotate(model, glm::radians(180.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));

	model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));	// it's a bit too big for our scene, so scale it down

	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	view = camera.GetViewMatrix();

	//Send MVP to Shader
	shader.use();
	shader.setMat4("model", model);
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

	//Update the bone positions
	for (uint i = 0; i < BonePosition.size(); i++)
	{
		BonePositionsMeshSpace.push_back(BonePosition[i] * Vector4f(0, 0, 0, 1));
		jointPosition[i] = { BonePositionsMeshSpace[i].x, BonePositionsMeshSpace[i].y, BonePositionsMeshSpace[i].z,1 };

	}

	//Send the positions to the buffer
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(jointPosition[0]) * 32, jointPosition, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);


	delete[] jointPosition;

}

void Skeleton::DrawSkeleton(Shader& shader)
{
	shader.use();
	glBindVertexArray(VAO);
	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, 32);
	glBindVertexArray(0);
}
#pragma once
#include "Shader.h"
#include "SkinnedMesh.h"

class Skeleton
{
public:
	Skeleton();
	void UpdateSkeletonBuffers(Shader& shader, vector<Matrix4f>& BonePosition, SkinnedMesh& model);
	void DrawSkeleton(Shader& shader);
	GLuint GetVAO() { return VAO; }

	glm::vec4* GetJointPostions()const;

private:
	//Actual joint positions
	glm::vec4* jointPosition;
	GLuint VAO, VBO;
};

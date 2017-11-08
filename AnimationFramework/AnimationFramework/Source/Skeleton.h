#pragma once
#include "Shader.h"
#include "SkinnedMesh.h"

class Skeleton
{
public:
	Skeleton();
	void UpdateSkeletonBuffers(Shader& shader, vector<Matrix4f>& BonePosition);
	void DrawSkeleton(Shader& shader);
	GLuint GetVAO() { return VAO; }

private:
	GLuint VAO, VBO;
};

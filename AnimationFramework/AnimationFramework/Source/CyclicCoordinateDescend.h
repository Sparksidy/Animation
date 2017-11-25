#pragma once

#include "SkinnedMesh.h"
#include "math_3d.h"
#include "Globals.h"


#define MAX_IK_TRIES 100
#define IK_POS_THRESH 1.0f

class CyclicCoordinateDescend {

public:
	CyclicCoordinateDescend();
	~CyclicCoordinateDescend();

	//True if the solution exists, False if its not within reach
	bool ComputeCCDLink(glm::vec3 target, SkinnedMesh &doom);

	
private:
	float VectorSquaredDistance(glm::vec3 pos1, glm::vec3 pos2);
	glm::vec3 targetPoint;
	
};

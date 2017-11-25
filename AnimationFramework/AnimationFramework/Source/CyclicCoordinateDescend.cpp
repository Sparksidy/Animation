#include "CyclicCoordinateDescend.h"


CyclicCoordinateDescend::CyclicCoordinateDescend()
{
	//TODO: Remove this later to dynamic target
	targetPoint = { 0, 10, 10 };
}

CyclicCoordinateDescend::~CyclicCoordinateDescend()
{
}

bool CyclicCoordinateDescend::ComputeCCDLink(glm::vec3 target, SkinnedMesh& doom)
{
	//TODO: Remove this later to dynamic target
	target = targetPoint;


	glm::vec3 rootPos, currEnd, desiredEnd, targetVector, curVector, crossResult;
	double cosAngle, turnAngle, turnDeg;
	int link,tries, Chainsize;

	tries = 0;
	Chainsize = doom.ChainLink.size() - 1 ;
	link = Chainsize - 1;
	

	do {
		//Get the rootPos
		rootPos.x = doom.ChainLink[link]->mTransformation[0][3];
		rootPos.y = doom.ChainLink[link]->mTransformation[1][3];
		rootPos.z = doom.ChainLink[link]->mTransformation[2][3];


		//Position of the end effector
		currEnd.x = doom.ChainLink[Chainsize]->mTransformation[0][3];
		currEnd.y = doom.ChainLink[Chainsize]->mTransformation[1][3];
		currEnd.z = doom.ChainLink[Chainsize]->mTransformation[2][3];


		//Desired End effector position
		desiredEnd.x = target.x;
		desiredEnd.y = target.y;
		desiredEnd.z = target.z;

		//Check if you are already close enough
		if (VectorSquaredDistance(currEnd, desiredEnd) > IK_POS_THRESH)
		{
			//Create a vector to the current effector pos
			curVector.x = currEnd.x - rootPos.x;
			curVector.y = currEnd.y - rootPos.y;
			curVector.z = currEnd.z - rootPos.z;

			//Create the desired effector position vector
			targetVector.x = target.x - rootPos.x;
			targetVector.y = target.y - rootPos.y;
			targetVector.z = target.z - rootPos.z;

			//Normalize the vectors
			glm::normalize(targetVector);
			glm::normalize(curVector);

			//Dot product gives me the cosine of the angle
			cosAngle = glm::dot(targetVector, curVector);

			//If dot is 1.0 we don't need to do anything as it is 0 degrees
			if (cosAngle < 0.99999f)
			{
				//Use the cross product to check which way to rotate
				crossResult = glm::cross(targetVector, curVector);

				//Rotate Clockwise
				turnAngle = acos((float)cosAngle);
				turnDeg = turnAngle * (180 / 3.14f);

				//TODO: DAMPING AND CLAMPING



				//Actually Rotate it
				float h = sin(turnDeg / 2);
				glm::normalize(crossResult);
				aiQuaternion Rotation(crossResult.x, crossResult.y, crossResult.z, h);

				doom.ChainLink[link]->mTransformation * (aiMatrix4x4)Rotation.GetMatrix();

				//TODO: DOF Restrictions




				


			}

			if (--link < 0)
				link = Chainsize - 1;

		}



	} while (tries++ < MAX_IK_TRIES &&
		VectorSquaredDistance(currEnd, desiredEnd) > IK_POS_THRESH);

	return true;

}

float CyclicCoordinateDescend::VectorSquaredDistance(glm::vec3 pos1, glm::vec3 pos2)
{
	return pow((pos2.x - pos1.x), 2) + pow((pos2.y - pos1.y), 2) + pow((pos2.z - pos1.z), 2);
}

#include "CyclicCoordinateDescend.h"
#include <algorithm>


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
	
	//So that it starts from root to the finger
	std::reverse(doom.ChainLink.begin(),doom.ChainLink.end());

	do {
		//Position of the end effector
		currEnd.x = doom.ChainLink[Chainsize]->mTransformation[0][3];
		currEnd.y = doom.ChainLink[Chainsize]->mTransformation[1][3];
		currEnd.z = doom.ChainLink[Chainsize]->mTransformation[2][3];


		//Get the rootPos
		rootPos.x = doom.ChainLink[link]->mTransformation[0][3];
		rootPos.y = doom.ChainLink[link]->mTransformation[1][3];
		rootPos.z = doom.ChainLink[link]->mTransformation[2][3];


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
			targetVector = glm::normalize(targetVector);
			curVector = glm::normalize(curVector);

			//Dot product gives me the cosine of the angle
			cosAngle = glm::dot(targetVector, curVector);

			//If dot is 1.0 we don't need to do anything as it is 0 degrees
			if (cosAngle < 0.99999f)
			{
				//ROTATE IT

				//Calculate the by how much to rotate
				turnAngle = acos((float)cosAngle);

				//Convert Radians to Degrees
				turnDeg = turnAngle * (180 / 3.14f);

				//TODO: CLAMPING
				if (turnDeg > 180)
					turnDeg = 180;

				//Use the cross product to check which way to rotate
				crossResult = glm::cross(targetVector, curVector);
				crossResult = glm::normalize(crossResult);


				aiQuaternion NewRotation(turnDeg,crossResult.x, crossResult.y, crossResult.z);


				Matrix4f NodeTransformation(doom.ChainLink[link]->mTransformation);
			
				//The 3x3 upper of mTransformation gives the orientation
				aiMatrix4x4 CurRotation;
				CurRotation[0][0] = doom.ChainLink[link]->mTransformation[0][0];
				CurRotation[0][1] = doom.ChainLink[link]->mTransformation[0][1];
				CurRotation[0][2] = doom.ChainLink[link]->mTransformation[0][2];
				CurRotation[0][3] = 0;
				CurRotation[1][0] = doom.ChainLink[link]->mTransformation[1][0];
				CurRotation[1][1] = doom.ChainLink[link]->mTransformation[1][1];
				CurRotation[1][2] = doom.ChainLink[link]->mTransformation[1][2];
				CurRotation[1][3] = 0;
				CurRotation[2][0] = doom.ChainLink[link]->mTransformation[2][0];
				CurRotation[2][1] = doom.ChainLink[link]->mTransformation[2][1];
				CurRotation[2][2] = doom.ChainLink[link]->mTransformation[2][2];
				CurRotation[2][3] = 0;
				CurRotation[3][0] = 0;
				CurRotation[3][1] = 0;
				CurRotation[3][2] = 0;
				CurRotation[3][3] = 1;
				Matrix4f Rotation = NewRotation.GetMatrix();

				//The 4th Column gives the translation vector
				aiVector3D CurTranslation;
				CurTranslation.x = doom.ChainLink[link]->mTransformation[0][3];
				CurTranslation.y = doom.ChainLink[link]->mTransformation[1][3];
				CurTranslation.z = doom.ChainLink[link]->mTransformation[2][3];
				Matrix4f Translation;
				Translation.InitTranslationTransform(CurTranslation.x, CurTranslation.y, CurTranslation.z);

				NodeTransformation = Translation * Rotation;

				//Concatenating with the parent transforms (FIXING THE HIERARCHY)
				Matrix4f GlobalTransformation;
				if(link != 0)
					GlobalTransformation = (Matrix4f)doom.ChainLink[link-1]->mTransformation * NodeTransformation;
				
				//Updating the bone transformations
				if (doom.m_BoneMapping.find(doom.ChainLink[link]->mName.data) != doom.m_BoneMapping.end())
				{
					unsigned int BoneIndex = doom.m_BoneMapping[doom.ChainLink[link]->mName.data];
					doom.m_BoneInfo[BoneIndex].FinalTransformation = doom.m_GlobalInverseTransform * GlobalTransformation * doom.m_BoneInfo[BoneIndex].BoneOffset;
					doom.m_BoneInfo[BoneIndex].BonePosition = doom.m_GlobalInverseTransform * GlobalTransformation;

				}

				//TODO: Degree Of Freedom Restrictions



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

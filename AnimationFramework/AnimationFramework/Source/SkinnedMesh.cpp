
#include "SkinnedMesh.h"
#include "Shader.h"
#include "Globals.h"

#define POSITION_LOCATION		0
#define TEX_COORD_LOCATION		1
#define NORMAL_LOCATION			2
#define BONE_ID_LOCATION		3
#define BONE_WEIGHT_LOCATION	4



float VectorSquaredDistance(aiVector3D pos1, aiVector3D pos2);


void SkinnedMesh::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
	for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	assert(0);
}

SkinnedMesh::SkinnedMesh()
{
	m_VAO = 0;
	ZERO_MEM(m_Buffers);
	m_NumBones = 0;
	m_Scene = NULL;
	tries = 0;

	


}

SkinnedMesh::~SkinnedMesh()
{
	Clear();
}


void SkinnedMesh::Clear()
{
	for (uint i = 0; i < m_Textures.size(); i++) {
		SAFE_DELETE(m_Textures[i]);
	}


	if (m_Buffers[0] != 0) {
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
	}

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}
GLint SkinnedMesh::GetUniformLocation(const char* pUniformName, GLuint shader)
{
	GLuint Location = glGetUniformLocation(shader, pUniformName);

	if (Location == INVALID_UNIFORM_LOCATION) {
		fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
	}

	return Location;
}
void SkinnedMesh::SetMVP(Shader& shader)
{
	//glm::vec3 COI = { 0,0,0 };
	//glm::vec3 W = glm::normalize(COI - modelsPosition);
	//glm::vec3 U = glm::cross(W, glm::vec3(0, 1, 0));
	//glm::vec3 V = glm::cross(U, W);

	//glm::mat4 orientation;
	//orientation[0][0] = U.x;	orientation[0][1] = U.y;	orientation[0][2] = U.z;	orientation[0][3] = 0;
	//orientation[1][0] = V.x;	orientation[1][1] = V.y;	orientation[1][2] = V.z;	orientation[1][3] = 0;
	//orientation[2][0] = W.x;	orientation[2][1] = W.y;	orientation[2][2] = W.z;	orientation[2][3] = 0;
	//orientation[3][0] = modelsPosition.x;		orientation[3][1] = modelsPosition.y;		orientation[3][2] = modelsPosition.z;		orientation[3][3] = 1;

	//
	//glm::mat4 model, view, projection;
	////model = glm::translate(model, modelsPosition); // translate it down so it's at the center of the scene
	////model = glm::translate(model, modelsPosition) * orientation * glm::translate(model, -modelsPosition);
	//											   
	//model = orientation;

	 glm::mat4 m_model, view, projection;
	m_model = glm::translate(m_model, modelsPosition);
	m_model = glm::rotate(m_model, glm::radians(90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
	m_model = glm::rotate(m_model, glm::radians(180.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
	m_model = glm::rotate(m_model, glm::radians(180.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
	
	m_model = glm::scale(m_model, glm::vec3(0.06f, 0.06f, 0.06f));	// it's a bit too big for our scene, so scale it down
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	view = camera.GetViewMatrix();

	shader.use();
	shader.setMat4("model", m_model);
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

}
void SkinnedMesh::UpdateBoneTransforms(vector<Matrix4f>& Transforms, vector<Matrix4f>& BonePosition, float RunningTime, float a)
{
	BoneTransform(RunningTime, Transforms, BonePosition, a);

	for (uint i = 0; i < Transforms.size(); i++) {
		SetBoneTransform(i, Transforms[i]);
	}

	
}

bool SkinnedMesh::LoadMesh(const string& filename)
{
	//Clear existing buffers
	Clear();

	//Create a VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	bool Ret = false;

	m_Scene = m_Importer.ReadFile(filename.c_str(), ASSIMP_LOAD_FLAGS);

	if (m_Scene)
	{
		m_GlobalInverseTransform = m_Scene->mRootNode->mTransformation;
		m_GlobalInverseTransform.Inverse();
		
		Ret = InitFromScene(m_Scene, filename);
	}
	else
	{
		printf("Error parsing '%s': '%s'\n", filename.c_str(), m_Importer.GetErrorString());
		return false;
	}


	glBindVertexArray(0);

	return Ret;

}

bool SkinnedMesh::InitFromScene(const aiScene* pScene, const string& filename)
{
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);


	vector<Vector3f> Positions;
	vector<Vector3f> Normals;
	vector<Vector2f> TexCoords;
	vector<unsigned int> Indices;
	vector<VertexBoneData> Bones;

	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;

	for (unsigned int i = 0; i < m_Entries.size(); i++)
	{
		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;

	}

	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Bones.resize(NumVertices);
	Indices.reserve(NumIndices);

	for (int i = 0; i < m_Entries.size(); i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
	}

	if (!InitMaterials(pScene, filename)) {
		return false;
	}

	//Generate and populate vertex buffers with attributes
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);


	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	

	return GLCheckError();

}

void SkinnedMesh::InitMesh(uint MeshIndex,
	const aiMesh* paiMesh,
	vector<Vector3f>& Positions,
	vector<Vector3f>& Normals,
	vector<Vector2f>& TexCoords,
	vector<VertexBoneData>& Bones,
	vector<uint>& Indices)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
	{
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		Positions.push_back(Vector3f(pPos->x, pPos->y, pPos->z));
		Normals.push_back(Vector3f(pNormal->x, pNormal->y, pNormal->z));
		TexCoords.push_back(Vector2f(pTexCoord->x, pTexCoord->y));

	}

	

	LoadBones(MeshIndex, paiMesh, Bones);

	//populate the index buffer

	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
	{
		const aiFace& face = paiMesh->mFaces[i];
		assert(face.mNumIndices == 3);
		Indices.push_back(face.mIndices[0]);
		Indices.push_back(face.mIndices[1]);
		Indices.push_back(face.mIndices[2]);
	}

	

}

bool SkinnedMesh::InitMaterials(const aiScene* pScene, const string& Filename)
{
	// Extract the directory part from the file name
	string::size_type SlashIndex = Filename.find_last_of("/");
	string Dir;

	if (SlashIndex == string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Initialize the materials
	for (uint i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		m_Textures[i] = NULL;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				string p(Path.data);

				if (p.substr(0, 2) == ".\\") {
					p = p.substr(2, p.size() - 2);
				}

				string FullPath = Dir + "/" + p;

				m_Textures[i] = new Textures(GL_TEXTURE_2D, FullPath.c_str());

				if (!m_Textures[i]->Load()) {
					printf("Error loading texture '%s'\n", FullPath.c_str());
					delete m_Textures[i];
					m_Textures[i] = NULL;
					Ret = false;
				}
				else {
					printf("%d - loaded texture '%s'\n", i, FullPath.c_str());
				}
			}
		}
	}

	return Ret;
}


void SkinnedMesh::LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
{
		
	for (unsigned int i = 0; i < pMesh->mNumBones; i++)
	{
		unsigned int BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		//Store the aiBone for right finger IK
		
		//If new bone
		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
		{
			//Allocate index for a new bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;


			m_BoneMapping[BoneName] = BoneIndex;

		}
		else
		{
			BoneIndex = m_BoneMapping[BoneName];
		}


		//Assigning weights
		for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++)
		{
			uint VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}

	}

	std::cout << std::endl;
	
}


void SkinnedMesh::Render(Shader& shader)
{
	shader.use();
	glBindVertexArray(m_VAO);

	for (uint i = 0; i < m_Entries.size(); i++) {
		
		const uint MaterialIndex = m_Entries[i].MaterialIndex;

		assert(MaterialIndex < m_Textures.size());

		if (m_Textures[MaterialIndex]) {
			m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
		}

		glDrawElementsBaseVertex(GL_TRIANGLES,
			m_Entries[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(uint) * m_Entries[i].BaseIndex),
			m_Entries[i].BaseVertex);
	}

	glBindVertexArray(0);

	
}

const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

void SkinnedMesh::ReadNodeHierarchy(float AnimationTime,  aiNode * pNode, const Matrix4f & ParentTransform)
{

	string Nodename(pNode->mName.data);

	if (Nodename == right_finger)
	{
		finger = pNode;
	}

	const aiAnimation* pAnim = m_Scene->mAnimations[0];
	
	Matrix4f NodeTransformation(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnim,Nodename);


	if (pNodeAnim) //If a node corresponds to bone do these transformations
	{
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		Matrix4f ScalingM;
		ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);


		aiQuaternion Q;
		CalcInterpolatedRotation(Q, AnimationTime, pNodeAnim);
		Matrix4f RotationM = Matrix4f(Q.GetMatrix());


		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		Matrix4f TranslationM;
		TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);


		NodeTransformation = TranslationM * RotationM * ScalingM;

	}

	Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;
	
	//Update the final transformation of the bone
	if (m_BoneMapping.find(Nodename) != m_BoneMapping.end())
	{
		
		unsigned int BoneIndex = m_BoneMapping[Nodename];

		
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
		m_BoneInfo[BoneIndex].BonePosition = m_GlobalInverseTransform * GlobalTransformation;
		m_BoneInfo[BoneIndex].GlobalTransformationMatrix = GlobalTransformation;

	}

	//Recursively update the bone transformation of all the childeren
	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}

	
}
float t = 0;
static bool flag = false;
void SkinnedMesh::BoneTransform(float timeInSeconds, vector<Matrix4f>& Transforms, vector<Matrix4f>& BonePosition,float a)
{
	Matrix4f Identity;
	Identity.InitIdentity();

	
	//Calculate the animation time
	float TicksPerSecond = m_Scene->mAnimations[0]->mTicksPerSecond > 0 ? m_Scene->mAnimations[0]->mTicksPerSecond  : 200.0f * a ;
	t += 0.16f * TicksPerSecond;
	//float TimeInTicks = timeInSeconds * TicksPerSecond;
	t = fmod(t, m_Scene->mAnimations[0]->mDuration);

	//Traverse the node hierarchy and update the final bone transformation according to the animation time
	//ReadNodeHierarchy(0, m_Scene->mRootNode, Identity);

	//IK

	ReadSkeleton(m_Scene->mRootNode, Identity);

	//ChainLink.clear();

	CreateSubChain();
	//CreateJointConstraints();
	
	if (!flag)
	{
		
		flag = true;
		ComputeCCD();
	}

	ReadSkeleton(m_Scene->mRootNode, Identity);

	

	//IK


	Transforms.resize(m_NumBones);
	BonePosition.resize(m_NumBones);
	
	for (unsigned int i = 0; i < m_NumBones; i++)
	{
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
		BonePosition[i] = m_BoneInfo[i].BonePosition;
	}

	
}

void SkinnedMesh::CreateSubChain()
{
	if (finger)
	{
		while (finger != m_Scene->mRootNode)
		{
			//If node name is the same as the bone name
			if (m_BoneMapping.find(finger->mName.data) != m_BoneMapping.end())
			{
				
				ChainLink.push_back(finger);

			}
			finger = finger->mParent;
		}
		
		ChainLink.push_back(m_Scene->mRootNode);
	}

}

void SkinnedMesh::CreateJointConstraints()
{
	for (int i = 0; i < ChainLink.size(); i++)
	{
		string name = ChainLink[i]->mName.data;

		if (name == right_finger)
		{
			JointConstraints.push_back(Constraints(0, 0));
		}
		else if (name == right_Hand)
		{
			JointConstraints.push_back(Constraints(0, 30));
		}
		else if (name == right_Forearm)
		{
			JointConstraints.push_back(Constraints(0, 30));
		}
		else if (name == right_UpperArm)
		{
			JointConstraints.push_back(Constraints(0, 10));
		}

		else if (name == right_Clavicle)
		{
			JointConstraints.push_back(Constraints(0, 20));
		}
		else if (name == Neck)
		{
			JointConstraints.push_back(Constraints(0, 30));
		}
		else if (name == Spine3)
		{
			JointConstraints.push_back(Constraints(0, 15));
		}
		else if (name == Spine2)
		{
			JointConstraints.push_back(Constraints(0, 15));
		}
		else if (name == Spine1)
		{
			JointConstraints.push_back(Constraints(0, 15));
		}
		else if (name == Spine)
		{
			JointConstraints.push_back(Constraints(0, 15));
		}
		else if (name == Pelvis)
		{
			JointConstraints.push_back(Constraints(0, 10));
		}
		else if (name == Root)
		{
			JointConstraints.push_back(Constraints(0, 0));

		}

	}

}

#define MAX_IK_TRIES		1
#define MIN_IK_THRESH		1.0f

void SkinnedMesh:: ComputeCCD()
{ 
	//Get the target point in World Space
	glm::vec3 target = {0, 10, 10};
	glm::vec3 model_position = this->GetModelsPosition();
	
	glm::mat4 model;
	model = glm::translate(model, model_position);
	model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
	model = glm::rotate(model, glm::radians(180.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
	model = glm::rotate(model, glm::radians(180.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
	model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));

	target = glm::vec3(glm::inverse(model) * glm::vec4(target,1.0));

	glm::vec3  TargetVector, jointToEEVector, crossResult;
	aiVector3D jointPos, EEPos, targetPos;
	aiMatrix4x4 jointPosWorldSpace, EEWorldSpace ;
	double cosAngle;
	int link = 1;

	
	do
	{
		// EE IN WORLD SPACE
		EEWorldSpace = GetWorldSpace(0);
		EEPos.x = EEWorldSpace[0][3];
		EEPos.y = EEWorldSpace[1][3];
		EEPos.z = EEWorldSpace[2][3];

		// JOINT POS IN WORLD SPACE
		jointPosWorldSpace = GetWorldSpace(link);
		jointPos.x = jointPosWorldSpace[0][3];
		jointPos.y = jointPosWorldSpace[1][3];
		jointPos.z = jointPosWorldSpace[2][3];

		//TARGET POS IN WORLD SPACE
		targetPos.x = target.x;
		targetPos.y = target.y;
		targetPos.z = target.z;

		if (VectorSquaredDistance(EEPos, targetPos) > 1.0)
		{
			// EE TO TARGET VECTOR
			TargetVector.x = targetPos.x - EEPos.x;
			TargetVector.y = targetPos.y - EEPos.y;
			TargetVector.z = targetPos.z - EEPos.z;

			// JOINT TO EE VECTOR
			jointToEEVector.x = EEPos.x - jointPos.x;
			jointToEEVector.y = EEPos.y - jointPos.y;
			jointToEEVector.z = EEPos.z - jointPos.z;


			TargetVector = glm::normalize(TargetVector);
			jointToEEVector = glm::normalize(jointToEEVector);


			// GET THE DOT PRODUCT BETWEEN TWO
			cosAngle = glm::dot(TargetVector, jointToEEVector);

			//THEN ROTATE IT
			if (cosAngle < 0.9899)
			{
			
				//CROSS PRODUCT TO GET AXIS
				crossResult = glm::cross(TargetVector, jointToEEVector);
				crossResult = glm::normalize(crossResult);





			}


		}




		

		
	} while (VectorSquaredDistance(EEPos, targetPos));

	


	

}

aiMatrix4x4 SkinnedMesh::GetWorldSpace(int index)
{
	aiMatrix4x4 WorldMatrix;
	for (int i = index; i < ChainLink.size() ; i++)
	{
		WorldMatrix = WorldMatrix * ChainLink[i]->mTransformation ;
	}

	return WorldMatrix;
}

//WASTE FUNCTIONS
void SkinnedMesh::ReadChainHiearchy(Matrix4f ParentGlobalTransform, int link)
{

	for (int i = link + 1; i < ChainLink.size(); i++)
	{
		if (m_BoneMapping.find(ChainLink[i]->mName.data) != m_BoneMapping.end())
		{

			unsigned int BoneIndex = m_BoneMapping[ChainLink[i]->mName.data];
			m_BoneInfo[BoneIndex].GlobalTransformationMatrix = ParentGlobalTransform * ChainLink[i]->mTransformation;
			m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * m_BoneInfo[BoneIndex].GlobalTransformationMatrix * m_BoneInfo[BoneIndex].BoneOffset;
			m_BoneInfo[BoneIndex].BonePosition = m_GlobalInverseTransform * m_BoneInfo[BoneIndex].GlobalTransformationMatrix;

			ParentGlobalTransform = m_BoneInfo[BoneIndex].GlobalTransformationMatrix;
		}
	}


}

//WASTE FUNCTIONS
void SkinnedMesh::ReadSkeletonCylinder()
{
	aiNode* currNode = m_Scene->mRootNode;
	unsigned int BoneIndex = 0;
	Matrix4f Identity;
	Identity.InitIdentity();

	while (currNode != nullptr)
	{
		
		string BoneName(currNode->mName.data);

		//If new bone
		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
		{
			//Allocate index for a new bone
			BoneIndex = m_NumBones;
			
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = Identity;


			m_BoneMapping[BoneName] = BoneIndex;

		}
		else
		{
			BoneIndex = m_BoneMapping[BoneName];
		}

		ChainLink.push_back(currNode);
		if (currNode->mChildren)
			currNode = currNode->mChildren[0];
		else
			break;
	}

	ReadChainHiearchy(Identity, 0);

		
}

aiMatrix4x4 SkinnedMesh::ToAiMatrix(Matrix4f matrix)
{
	aiMatrix4x4 newMatrix;
	newMatrix[0][0] = matrix.m[0][0];
	newMatrix[0][1] = matrix.m[0][1];
	newMatrix[0][2] = matrix.m[0][2];
	newMatrix[0][3] = matrix.m[0][3];
	newMatrix[1][0] = matrix.m[1][0];
	newMatrix[1][1] = matrix.m[1][1];
	newMatrix[1][2] = matrix.m[1][2];
	newMatrix[1][3] = matrix.m[1][3];
	newMatrix[2][0] = matrix.m[2][0];
	newMatrix[2][1] = matrix.m[2][1];
	newMatrix[2][2] = matrix.m[2][2];
	newMatrix[2][3] = matrix.m[2][3];
	newMatrix[3][0] = matrix.m[3][0];
	newMatrix[3][1] = matrix.m[3][1];
	newMatrix[3][2] = matrix.m[3][2];
	newMatrix[3][3] = matrix.m[3][3];
							
	return newMatrix;
}

aiVector3D SkinnedMesh::CalculateTranslationFromMatrix(aiMatrix4x4 matrix)
{
	aiVector3D CurTranslation;
	CurTranslation.x = matrix[0][3];
	CurTranslation.y = matrix[1][3];
	CurTranslation.z = matrix[2][3];
	
	return CurTranslation;
}

aiMatrix4x4 SkinnedMesh::CalculateRotationFromMatrix(aiMatrix4x4 matrix)
{
	aiMatrix4x4 CurRotation;

	CurRotation[0][0] = matrix[0][0];
	CurRotation[0][1] = matrix[0][1];
	CurRotation[0][2] = matrix[0][2];
	CurRotation[0][3] = 0;
	CurRotation[1][0] = matrix[1][0];
	CurRotation[1][1] = matrix[1][1];
	CurRotation[1][2] = matrix[1][2];
	CurRotation[1][3] = 0;
	CurRotation[2][0] = matrix[2][0];
	CurRotation[2][1] = matrix[2][1];
	CurRotation[2][2] = matrix[2][2];
	CurRotation[2][3] = 0;
	CurRotation[3][0] = 0;
	CurRotation[3][1] = 0;
	CurRotation[3][2] = 0;
	CurRotation[3][3] = 1;

	return CurRotation;
	
}

void SkinnedMesh::ReadSkeleton(aiNode * pNode, const Matrix4f & ParentTransform)
{
	string Nodename(pNode->mName.data);

	if (Nodename == right_finger)
	{
		finger = pNode;
	}

	Matrix4f GlobalTransformation = ParentTransform * pNode->mTransformation;

	//Update the final transformation of the bone
	if (m_BoneMapping.find(Nodename) != m_BoneMapping.end())
	{

		unsigned int BoneIndex = m_BoneMapping[Nodename];


		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
		m_BoneInfo[BoneIndex].BonePosition = m_GlobalInverseTransform * GlobalTransformation;
		m_BoneInfo[BoneIndex].GlobalTransformationMatrix = GlobalTransformation;

	}

	//Recursively update the bone transformation of all the childeren
	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadSkeleton( pNode->mChildren[i], GlobalTransformation);
	}
}


float VectorSquaredDistance(aiVector3D pos1, aiVector3D pos2)
{
	float a = sqrt(pow((pos2.x - pos1.x), 2) + pow((pos2.y - pos1.y), 2) + pow((pos2.z - pos1.z), 2));
	return a;
}

void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	//assert(Factor >= -0.1f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//assert(Factor >= -0.2f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	//assert(Factor >= -0.1f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

uint SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

void SkinnedMesh::SetBoneTransform(int index, const Matrix4f & Transform)
{
	glUniformMatrix4fv(m_boneLocation[index], 1, GL_TRUE, (const GLfloat*)Transform);
}

void SkinnedMesh::SendBonesLocationToShader(const Shader& shader)
{
	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_boneLocation); i++) {
		char Name[128];
		memset(Name, 0, sizeof(Name));
		SNPRINTF(Name, sizeof(Name), "gBones[%d]", i);
		m_boneLocation[i] = GetUniformLocation(Name, shader.ID);
	}

}

uint SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

uint SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}





#pragma once
#include <glew.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>			// Output data structure
#include <assimp/postprocess.h>


#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>


#include "util.h"
#include "math_3d.h"
#include "Textures.h"


#include <string>
#include <vector>
#include <map>

using namespace std;

class Shader;

class SkinnedMesh {

public:
	vector<Matrix4f> m_BonePosition;


#define MAX_BONES 100

	SkinnedMesh() ;
	~SkinnedMesh();

	bool LoadMesh(const string& Filename);

	void Render(Shader& shader);

	unsigned int NumBones()const
	{
		return m_NumBones;
	}

	void BoneTransform(float timeInSeconds, vector<Matrix4f>& Transforms, vector<Matrix4f>& BonePosition, float a);
	GLint GetUniformLocation(const char* pUniformName, GLuint shader );
	void SetBoneTransform(int index, const Matrix4f& Transform);
	void SendBonesLocationToShader(const Shader& shader);
	void SetMVP( Shader& shader);
	void UpdateBoneTransforms(vector<Matrix4f>& Transforms, vector<Matrix4f>& BonePosition, float RunningTime, float a);

	
	glm::vec3 modelsPosition = {0,0,0};
	glm::vec3 GetModelsPosition() { return modelsPosition; }
	void SetModelsPosition(Vector3f v) {
		modelsPosition.x = v.x;
		modelsPosition.y = v.y;
		modelsPosition.z = v.z;

	}

	glm::vec3 COI;
	void SetCOI(Vector3f coi) { 
		COI.x = coi.x;
		COI.y = coi.y;
		COI.z = coi.z;
	}
	glm::vec3 GetCOI() { return COI; }

private:
	GLuint m_boneLocation[MAX_BONES];
	
	#define NUM_BONES_PER_VERTEX 4

	struct BoneInfo {

		Matrix4f BoneOffset;
		Matrix4f FinalTransformation;
		Matrix4f BonePosition;
		Matrix4f GlobalTransformationMatrix;

		BoneInfo()
		{
			BoneOffset.SetZero();
			FinalTransformation.SetZero();
			BonePosition.SetZero();
			GlobalTransformationMatrix.SetZero();
		}
	};

	struct VertexBoneData {
		unsigned int IDs[NUM_BONES_PER_VERTEX];
		float Weights[NUM_BONES_PER_VERTEX];

		VertexBoneData()
		{
			Reset();
		}

		void Reset()
		{
			ZERO_MEM(IDs);
			ZERO_MEM(Weights);
		}

		void AddBoneData(unsigned int BoneID, float weight);
	};

	
	bool InitFromScene(const aiScene* pScene, const string& Filename);
	void InitMesh(uint MeshIndex,
		const aiMesh* paiMesh,
		vector<Vector3f>& Positions,
		vector<Vector3f>& Normals,
		vector<Vector2f>& TexCoords,
		vector<VertexBoneData>& Bones,
		vector<unsigned int>& Indices);

	void LoadBones(uint MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);
	
	bool InitMaterials(const aiScene* pScene, const string& Filename);

	void Clear();

	void ReadNodeHierarchy(float AnimationTime,  aiNode* pNode, const Matrix4f& ParentTransform);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);


	//IK
	public:
		void CreateSubChain();
		void ComputeCCD();
		aiMatrix4x4 GetWorldSpace(int index);
		void ReadChainHiearchy(Matrix4f BonePosition, int link);
		void ReadSkeletonCylinder();
		aiMatrix4x4 ToAiMatrix(Matrix4f matrix);
		aiVector3D CalculateTranslationFromMatrix(aiMatrix4x4 matrix);
		aiMatrix4x4 CalculateRotationFromMatrix(aiMatrix4x4 matrix);
		void ReadSkeleton(aiNode* pNode, const Matrix4f & ParentTransform);

		//Number of IK Tries
		int tries;

		string right_finger = "Bip01_R_Finger1";
		std::vector<aiNode*> ChainLink;
		aiNode* finger = nullptr;

		std::map<string, unsigned int> m_BoneMapping;
		vector<BoneInfo> m_BoneInfo;
		Matrix4f m_GlobalInverseTransform;
		
	//IK
	private:


#define INVALID_MATERIAL 0xFFFFFFFF

	enum VB_TYPES {
		INDEX_BUFFER,
		POS_VB,
		NORMAL_VB,
		TEXCOORD_VB,
		BONE_VB,

		NUM_VBs
	};

	GLuint m_VAO;
	GLuint m_Buffers[NUM_VBs];

	struct Meshentry {
		Meshentry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;

	};

	vector<Meshentry> m_Entries;
	vector<Textures*> m_Textures;


	unsigned int m_NumBones;
	/*vector<BoneInfo> m_BoneInfo;
	Matrix4f m_GlobalInverseTransform;*/

	const aiScene* m_Scene;
	Assimp::Importer m_Importer;






};

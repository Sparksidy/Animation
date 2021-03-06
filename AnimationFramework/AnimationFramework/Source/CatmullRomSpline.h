#pragma once
#include <glew.h>

#include "Globals.h"
#include "math_3d.h"
#include "Shader.h"
#include "SkinnedMesh.h"



class CatMullRomSpline{

	GLuint VAO, VBO;

	float maxArcLength;
	float maxParametricValue;
	float step			 =		0.005f;
	float speed			 =		10.0f;
	float velocity		 =		2.0f;
	const float t1		 =		0.10f;
	const float t2		 =		0.70f;


	
	Vector3f v0 = { 10.0, 0.0, 40.0 };
	Vector3f v1 = { 30.0, 0.0, 20.0 };
	Vector3f v2 = { 40.0, 0.0, -10.0 };
	Vector3f v3 = { 30.0, 0.0, -20.0 };
	Vector3f v4 = { 10.0, 0.0, -40.0 };
	Vector3f v5 = { -20.0, 0.0, -40.0 };
	Vector3f v6 = { -35.0, 0.0, -25.0 };
	Vector3f v7 = { -25.0, 0.0, 20.0 };
	Vector3f v8 = { -10.0, 0.0, 30.0 };
	Vector3f v9 = { -5.0, 0.0, 35.0 };
	Vector3f v10 = { 0.0, 0.0, 40.0 };
	Vector3f v11 = { 10.0, 0.0, 40.0 };
	Vector3f v12 = { 30.0, 0.0, 20.0 };
	Vector3f v13 = { 40.0, 0.0, -10.0 };
	Vector3f v14 = { 30.0, 0.0, -20.0 };

	
	//Forward Differencing Table
	struct TableEntry {
		float ParametricValue;
		float ArcLength;
		int index;
	};
	std::vector<TableEntry> _Table;

	//To know at what step value we have which Point on the curve
	//struct CurveSegmentPointTable {
	//	float parameter;
	//	Vector3f point;
	//	int index;			//Starting point/Determines the segment of the curve we are on
	//};
	//std::vector<CurveSegmentPointTable> _Map;

	
	
	public:
		float GetVelocity() { return velocity; }
		float GetSpeed() { return speed; }

		std::vector<Vector3f> _interpolatedPoints;
		std::vector<Vector3f> _controlPoints;

		CatMullRomSpline();
		~CatMullRomSpline();

		void DesignCurve();
		void DesignTable();
		void DrawCurve(Shader& shader);

		Vector3f CatMullRom(Vector3f *pout, Vector3f *pv0, Vector3f *pv1, Vector3f *pv2, Vector3f *pv3, float s);

		void UpdateMVP(Shader& shader);
		void FillBuffers();

		//Calculates the point on the curve with the distance travelled
		void Update(float dt, SkinnedMesh& model, float deltaTime);

		float GetParameterFromArcLength(float distance, int& index);
		float BinarySearch(int l, int r, float x);

	
		void PrintTable();
};

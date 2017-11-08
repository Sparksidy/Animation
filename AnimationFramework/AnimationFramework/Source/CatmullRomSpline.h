#pragma once
#include <glew.h>

#include "Globals.h"
#include "math_3d.h"
#include "Shader.h"

class CatMullRomSpline{
	
	/*Vector3f v0 = { -40.0, 0.0, -40.0 };
	Vector3f v1 = { -20.0, 0.0, 40.0 };
	Vector3f v2 = { 20.0, 0.0, -40.0 };
	Vector3f v3 = { 40.0, 0.0, 40.0 };*/

	Vector3f v0 = { 10.0, 0.0, 40.0 };
	Vector3f v1 = { 30.0, 0.0, 20.0 };
	Vector3f v2 = { 40.0, 0.0, -10.0 };
	Vector3f v3 = { 30.0, 0.0, -20.0 };
	Vector3f v4 = { 10.0, 0.0, -40.0 };
	Vector3f v5 = { -20.0, 0.0, -40.0 };
	Vector3f v6 = { -35.0, 0.0, -25.0 };
	Vector3f v7 = { -25.0, 0.0, 20.0 };
	Vector3f v8 = { -10.0, 0.0, 30.0 };
	Vector3f v9 = { -8.0, 0.0, 35.0 };
	Vector3f v10 = { 0.0, 0.0, 40.0 };
	Vector3f v11 = { 10.0, 0.0, 40.0 };
	Vector3f v12 = { 10.0, 0.0, 40.0 };
	Vector3f v13 = { 10.0, 0.0, 40.0 };
	Vector3f v14 = { 30.0, 0.0, 20.0 };





	float step = 0.001f;

	GLuint VAO, VBO;

	public:

		std::vector<Vector3f> _interpolatedPoints;
		std::vector<Vector3f> _controlPoints;

		CatMullRomSpline();
		~CatMullRomSpline();

		void DesignCurve();
		void DrawCurve(Shader& shader);

		Vector3f CatMullRom(Vector3f *pout, Vector3f *pv0, Vector3f *pv1, Vector3f *pv2, Vector3f *pv3, float s);

		void UpdateMVP(Shader& shader);
		void FillBuffers();
};

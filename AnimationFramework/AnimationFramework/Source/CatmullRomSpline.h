#pragma once
#include <glew.h>

#include "Globals.h"
#include "math_3d.h"
#include "Shader.h"

class CatMullRomSpline{
	
	Vector3f v0 = { -4.0, 0.0, -4.0 };
	Vector3f v1 = { -2.0, 0.0, 4.0 };
	Vector3f v2 = { 2.0, 0.0, -4.0 };
	Vector3f v3 = { 4.0, 0.0, 4.0 };

	float step = 0.01f;

	GLuint VAO, VBO;

	public:

		Vector3f interpolatedPoints[150];

		CatMullRomSpline();
		~CatMullRomSpline();

		void DesignCurve();
		void DrawCurve(Shader& shader);

		Vector3f CatMullRom(Vector3f *pout, Vector3f *pv0, Vector3f *pv1, Vector3f *pv2, Vector3f *pv3, float s);

		void UpdateMVP(Shader& shader);
		void FillBuffers();
};

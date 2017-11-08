#include "CatmullRomSpline.h"


CatMullRomSpline::CatMullRomSpline()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(0);
}

CatMullRomSpline::~CatMullRomSpline()
{
	
}

void CatMullRomSpline::DesignCurve()
{
	Vector3f vec;
	int counter = 0;
	for (float i = 0.0f; i <= 1.0f; i += step)
	{
		Vector3f point = CatMullRom(&vec, &v0, &v1, &v2, &v3, i);
		interpolatedPoints[counter++] = point;
	}
}
Vector3f CatMullRomSpline::CatMullRom(Vector3f *pout, Vector3f *pv0, Vector3f *pv1, Vector3f *pv2, Vector3f *pv3, float s)
{
	pout->x = 0.5f * (2.0f * pv1->x + (pv2->x - pv0->x) *s + (2.0f *pv0->x - 5.0f * pv1->x + 4.0f * pv2->x - pv3->x) * s * s + (pv3->x - 3.0f * pv2->x + 3.0f * pv1->x - pv0->x) * s * s * s);
	pout->y = 0.5f * (2.0f * pv1->y + (pv2->y - pv0->y) *s + (2.0f *pv0->y - 5.0f * pv1->y + 4.0f * pv2->y - pv3->y) * s * s + (pv3->y - 3.0f * pv2->y + 3.0f * pv1->y - pv0->y) * s * s * s);
	pout->z = 0.5f * (2.0f * pv1->z + (pv2->z - pv0->z) *s + (2.0f *pv0->z - 5.0f * pv1->z + 4.0f * pv2->z - pv3->z) * s * s + (pv3->z - 3.0f * pv2->z + 3.0f * pv1->z - pv0->z) * s * s * s);
	return (*pout);
}
void CatMullRomSpline::DrawCurve(Shader& shader)
{
	shader.use();
	glBindVertexArray(VAO);
	glPointSize(2.5f);
	glDrawArrays(GL_POINTS, 0, 150);
	glBindVertexArray(0);
}



void CatMullRomSpline::UpdateMVP(Shader& shader)
{
	glm::mat4 model, projection, view;
	model = glm::translate(model, glm::vec3(0.0f, 0.1f, 0.0f));
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	view = camera.GetViewMatrix();

	shader.use();
	shader.setMat4("model", model);
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
}

void CatMullRomSpline::FillBuffers()
{
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f) * 150, &interpolatedPoints[0], GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,3 * sizeof(float), (void*)0);
	
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	

}

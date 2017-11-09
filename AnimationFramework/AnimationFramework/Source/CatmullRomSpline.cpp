#include "CatmullRomSpline.h"


CatMullRomSpline::CatMullRomSpline()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(0);

	_controlPoints.push_back(v0);
	_controlPoints.push_back(v1);
	_controlPoints.push_back(v2);
	_controlPoints.push_back(v3);
	_controlPoints.push_back(v4);
	_controlPoints.push_back(v5);
	_controlPoints.push_back(v6);
	_controlPoints.push_back(v7);
	_controlPoints.push_back(v8);
	_controlPoints.push_back(v9);
	_controlPoints.push_back(v10);
	_controlPoints.push_back(v11);
	_controlPoints.push_back(v12);
	_controlPoints.push_back(v13);
	_controlPoints.push_back(v14);



}

CatMullRomSpline::~CatMullRomSpline()
{
	
}

void CatMullRomSpline::DesignCurve()
{
	Vector3f vec;
	
	for(int j=0; j<_controlPoints.size()-3 ; j++)
	{ 
		for (float i = 0.0f; i <= 1.0f; i += step)
		{
			Vector3f point = CatMullRom(&vec, &_controlPoints[j], &_controlPoints[j + 1], &_controlPoints[j + 2], &_controlPoints[j + 3], i);
			_interpolatedPoints.push_back(point);

			//Store the point with respect to the step value on the curve
			VectorsStepMap v;
			v.parameter = i;
			v.point = point;
			_Map.push_back(v);
		}
	}

}
void CatMullRomSpline::DesignTable()
{
	int counter = 0;
	for (float i = 0; i <= 1.0f; i += step)
	{
		TableEntry t;
		t.ParametricValue = i;
		
		//Calculate the arc Length
		if (i == 0.0f)
		{
			
			//Initial Arc Length
			t.ArcLength = 0;
			counter++;
		}
		else
		{
			//Get the points on the curve with the step value, P(0) & P(0.05)
			Vector3f point1 = _Map[counter].point;
			Vector3f point2 = _Map[counter - 1].point;


			//Calculate the distance between Pi and Pi-1;
			float distance = abs(sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2) + pow(point1.z - point2.z, 2)));

			//Get the last entry of the vector
			TableEntry back = _Table.back();

			
			//Distance between Pi and Pi-1 plus the previous length
			t.ArcLength = back.ArcLength + distance;

		}

		//Store it in the table
		_Table.push_back(t);
		counter++;

	}

	float maxLength = _Table.back().ArcLength;

	//Normalize the table's Arc Length
	for (int i = 0; i < _Table.size(); i++)
	{
		if (_Table[i].ArcLength != 0.0f)
		{
			_Table[i].ArcLength = _Table[i].ArcLength / maxLength;
		}
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
	glDrawArrays(GL_POINTS, 0,_interpolatedPoints.size());
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
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f) * _interpolatedPoints.size(), &_interpolatedPoints[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	

}

void CatMullRomSpline::PrintTable()
{
	for (int i = 0; i < _Table.size(); i++)
	{
		std::cout << "Parameter: " << _Table[i].ParametricValue << " Arc Length " << _Table[i].ArcLength << std::endl;
	}
}

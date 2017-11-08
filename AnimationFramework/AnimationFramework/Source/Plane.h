#pragma once
#include <glew.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Plane {

private:
	GLuint planeVAO;
	GLuint planeVBO;

	float planeVertices[48] = {
		// positions            // normals         // texcoords
		50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,  25.0f, 10.0f
	};
	glm::vec3 plane_position = { 0, 0, 0 };

public:
	Plane() {
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 48, planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);

	}

	~Plane()
	{
		glDeleteVertexArrays(1, &planeVAO);
		glDeleteBuffers(1, &planeVBO);
	}

	void Render(Shader& shader)
	{
		shader.use();
		glm::mat4 model;
		model = glm::translate(model, plane_position);
		shader.setMat4("model", model);
		glm::mat4 projection, view;
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		view = camera.GetViewMatrix();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 8);

	}




	

};
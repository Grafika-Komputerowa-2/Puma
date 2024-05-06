#pragma once
#include "Mesh.h"
#include "AszkalMeshReader.h"
#include "OpenGLHelper.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

class Robot:public Mesh
{
	const int numberOfParts = 6;

	RobotsPartInfo partsInfo[6];

	glm::mat4 partsModel_Mtx[6];

	VAO partsVAO_triangles[6];
	VAO partsVAO_edges[6];

	Shader trianglesShader;
	Shader edgesShader;

	glm::vec4 partsColors[6] = {
		{1, 0.5, 1, 1},
		{0.5, 0.5, 1, 1},
		{1, 0.5, 0.5, 1},
		{0.5, 1, 0.5, 1},
		{0.5, 1, 1, 1},
		{1, 1, 0.5, 1}
	};
public:
	void virtual Draw(GLFWwindow* window, const Camera& camera);
	Robot();
};


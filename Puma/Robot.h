#pragma once
#include "Mesh.h"
#include "AszkalMeshReader.h"
#include "OpenGLHelper.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Light.h"

class Robot:public Mesh
{
	const int numberOfParts = 6;

	RobotsPartInfo partsInfo[6];

	glm::mat4 partsModel_Mtx[6];

	VAO partsVAO_triangles[6];
	VAO partsVAO_edges[6];

	Shader trianglesShader;
	Shader edgesShader;

	glm::vec3 robotColor = { 0.85, 0.85, 0.85 };
public:
	void virtual Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount);
	Robot();
};


#pragma once
#include "Camera.h"
#include "VAO.h"
#include "EBO.h"
#include "Light.h"
#include "OpenGLHelper.h"

class Cylinder
{
	int numberOfIndices;
	Shader shader_cilinder;
	VAO vao_cilinder;
	glm::mat4 cilinder_modelMtx{ 1.0f };
	glm::vec3 color{ 0.5, 0.5, 1 };

	std::vector<float> GetUnitCircleVertices(float sectorCount);
	void BuildVerticesSmooth(float sectorCount);
	glm::vec3 CylinderVertex(float teta, float t);
public:
	Cylinder(float radius, float height, int sample, glm::vec3 pos, float yRotation);
	void Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount);

};


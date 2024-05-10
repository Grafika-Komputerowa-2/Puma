#pragma once
#include "ScensObject.h"
#include "Spark.h"

class particionGeneration: public ScensObject
{
	const int sparkNumber;
	Spark* sparks;
public:
	particionGeneration(int n);
	void virtual Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount, glm::mat4 trans = glm::mat4{ 1.0f });
	void Update(float scalar, glm::vec3 newStart, glm::vec3 downDirection);
};


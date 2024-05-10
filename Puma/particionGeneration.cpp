#include "particionGeneration.h"

particionGeneration::particionGeneration(int n): sparkNumber{n}
{
	sparks = new Spark[n];
}

void particionGeneration::Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount, glm::mat4 trans)
{
	for (int i = 0; i < sparkNumber; i++)
	{
		sparks[i].Draw(window, camera, lights, lightsCount, trans);
	}
}

void particionGeneration::Update(float scalar, glm::vec3 newStart, glm::vec3 downDirection)
{
	for (int i = 0; i < sparkNumber; i++)
	{
		sparks[i].startPos = newStart;
		sparks[i].velocity += downDirection * scalar;
		sparks[i].UpdatePos(scalar * 2);
	}
}

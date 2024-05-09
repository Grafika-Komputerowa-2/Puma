#pragma once
#include "StaticShaders.h"
class ScensObject
{
protected:
	Shader shader;
	float alfa = 1;
public:
	void virtual Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount, glm::mat4 trans = glm::mat4{ 1.0f }){}
	Shader SwapShaders(Shader newShader);
};


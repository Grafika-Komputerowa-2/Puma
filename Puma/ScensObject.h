#pragma once
class ScensObject
{
public:
	void virtual Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount, glm::mat4 trans = glm::mat4{ 1.0f }){}
};


#pragma once
#include "ScensObject.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include "EBO.h"

class Spark: public ScensObject
{
	glm::vec3 lastPos;
	glm::vec3 pos;
	double birthTime;
	double maxAge ;
	VAO vao;

public:
	glm::vec3 startPos{ 0, 0, 0 };
	glm::vec3 velocity;

	void Reset();
	Spark(); 
	glm::vec3 RandomStartDirection();
	void virtual Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount, glm::mat4 trans = glm::mat4{ 1.0f });
	void UpdatePos(float scalar = 1);
};


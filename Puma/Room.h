#pragma once
#include "Camera.h"
#include "VAO.h"
#include "Light.h"
#include "Robot.h"
#include "Mirror.h"
#include "Cylinder.h"
#include "ScensObject.h"

class Room: public ScensObject
{
	Shader shader_wall;
	VAO vao_wall;
	glm::mat4 walls_modelMtx[6]{};
	glm::vec4 wall_colors[6] = { 
		{0, 0.5, 0, 1}, 
		{0.5, 0.5, 0, 1},
		{0, 0.5, 0.5, 1},
		{0.5, 0, 0.5, 1},
		{0.5, 0, 0, 1},
		{0, 0, 0.5, 1}
	};

	Cylinder cylinder{ 0.5f, 10, 20, {0, -0.5, -3}, 90 };
	Robot robot;
	Mirror mirror{ 1, 2, glm::vec3{-1.5f, 0.4, 0} };
	bool animationOn = false;

	const int numberOfObjects = 3;
	ScensObject* objectsToDraw[3]{ &robot, &cylinder, this };
	
public:
	Room(float wdith, float height, float deep);
	void virtual Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount, glm::mat4 trans = glm::mat4{ 1.0f });
	void Animation();
	void UserInterfers();
	void  DrawAll(GLFWwindow* window, const Camera& camera, Light* lights, int lightCount);

};


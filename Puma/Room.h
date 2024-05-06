#pragma once
#include "Mesh.h"
#include "VAO.h"
class Room: public Mesh
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
public:
	Room(float wdith, float height, float deep);
	void virtual Draw(GLFWwindow* window, const Camera& camera);
};


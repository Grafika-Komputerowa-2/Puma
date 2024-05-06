#pragma once
#include "Mesh.h"
#include "VAO.h"
class Room: public Mesh
{
	Shader shader_wall;
	VAO vao_wall;
	glm::mat4 walls_modelMtx[6];
	glm::vec4 wall_color = { 0, 0.5, 0, 1 };
public:
	Room(float size);
	void virtual Draw(GLFWwindow* window, const Camera& camera);
};


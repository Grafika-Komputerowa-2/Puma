#pragma once
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <ImGui/imgui_impl_glfw.h>
#include "Camera.h"
#include "Light.h"

class Mirror
{
	Shader shader_mirror;
	VAO vao_mirror;
	glm::mat4 mirror_modelMtx{ 1.0f };
	glm::vec3 mirror_color{ 1, 0.5, 0.5 };
public:
	const glm::vec3 normal = { 0, 0, 1 };
	glm::mat4 GetModelMtx();
	Mirror(float width, float height, glm::vec3 pos);
	void Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount);
};


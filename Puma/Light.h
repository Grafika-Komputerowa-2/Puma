#pragma once
#include <glm/ext/vector_float3.hpp>

struct Light {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

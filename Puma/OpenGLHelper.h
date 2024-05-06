#pragma once
#include <glm/ext/vector_float3.hpp>
#include <vector>

static class OpenGLHelper {
public:
	static void AddVecToVector(std::vector<float>& vector, const glm::vec3& pos) {
		vector.push_back(pos.x); vector.push_back(pos.y); vector.push_back(pos.z);
	}

	static glm::vec3 TakeLastVecFromVector(std::vector<float>& vector) {
		glm::vec3 pos{ vector.at(vector.size() - 3), vector.at(vector.size() - 2), vector.at(vector.size() - 1) };
		vector.pop_back();
		vector.pop_back();
		vector.pop_back();

		return pos;
	}

	static glm::vec3 TakeCopyOfVec3(std::vector<float>& vector, int id, int offset, int size) {
		glm::vec3 pos{
			vector.at(id * size + offset),
			vector.at(id * size + 1 + offset), 
			vector.at(id * size + 2 + offset) };
		return pos;
	}

	template<typename T>
	static T TakeOneCopyOf(std::vector<T>& vector, int id, int offset, int size) {
		return vector.at(id * size + offset);
	}
};
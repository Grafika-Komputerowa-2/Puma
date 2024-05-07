#include "Room.h"
#include "EBO.h"

Room::Room(float width, float height, float deep)
{
	const float halfWidth = width * 0.5f;
	const float halfHeight = height * 0.5f;
	const float halfDeep = deep * 0.5f;

	// Location, Normal, Color
	std::vector<float> wallVs = { 
		-1, -1, 0, 0, 0, -1, .8f, .8f, .8f,
		-1, +1, 0, 0, 0, -1, .8f, .2f, .8f,
		+1, +1, 0, 0, 0, -1, .8f, .2f, .8f,
		+1, -1, 0, 0, 0, -1, .8f, .8f, .8f
	};

	std::vector<GLuint> wallIes = {
		0, 1, 2,
		0, 2, 3
	};

	vao_wall.Bind();
	VBO vbo(wallVs, GL_STATIC_DRAW);
	EBO ebo(wallIes);

	vao_wall.LinkAttrib(0, 3, GL_FLOAT, false, 9 * sizeof(float), 0);
	vao_wall.LinkAttrib(1, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	vao_wall.LinkAttrib(2, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)(6 * sizeof(float)));

	vao_wall.Unbind(); vbo.Unbind(); ebo.Unbind();

	shader_wall = { "phong_vert.glsl", "phong_frag.glsl" };

	float startPos_y = halfHeight - 1;
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0, 0.0, -halfDeep));
	trans = glm::translate(trans, glm::vec3(0.0, startPos_y, 0.0));
	trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));
	trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(halfWidth, halfHeight, 1));
	walls_modelMtx[0] = trans;

	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0, 0.0, +halfDeep));
	trans = glm::translate(trans, glm::vec3(0.0, startPos_y, 0.0));
	trans = glm::scale(trans, glm::vec3(halfWidth, halfHeight, 1));
	walls_modelMtx[1] = trans;

	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0, -halfHeight, 0.0));
	trans = glm::translate(trans, glm::vec3(0.0, startPos_y, 0.0));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	trans = glm::scale(trans, glm::vec3(halfWidth, halfDeep, 1));
	walls_modelMtx[2] = trans;

	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0, +halfHeight, 0.0));
	trans = glm::translate(trans, glm::vec3(0.0, startPos_y, 0.0));
	trans = glm::rotate(trans, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	trans = glm::scale(trans, glm::vec3(halfWidth, halfDeep, 1));
	walls_modelMtx[3] = trans;

	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(-halfWidth, 0.0, 0.0));
	trans = glm::translate(trans, glm::vec3(0.0, startPos_y, 0.0));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	trans = glm::scale(trans, glm::vec3(halfDeep, halfHeight, 1));
	walls_modelMtx[4] = trans;

	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(+halfWidth, 0.0, 0.0));
	trans = glm::translate(trans, glm::vec3(0.0, startPos_y, 0.0));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	trans = glm::scale(trans, glm::vec3(halfDeep, halfHeight, 1));
	walls_modelMtx[5] = trans;
}

struct Light {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};
void loadLightUniform(GLuint shaderProgram, const Light* lights, int lightCount) {
	for (int i = 0; i < lightCount; i++) {
		std::string uniformName = "lights[" + std::to_string(i) + "]";
		// Get the location of the uniform variables
		GLint posLoc = glGetUniformLocation(shaderProgram, (uniformName + ".position").c_str());
		GLint ambLoc = glGetUniformLocation(shaderProgram, (uniformName + ".ambient").c_str());
		GLint diffLoc = glGetUniformLocation(shaderProgram, (uniformName + ".diffuse").c_str());
		GLint specLoc = glGetUniformLocation(shaderProgram, (uniformName + ".specular").c_str());
		// Set the values of the uniform variables
		glUniform3f(posLoc, lights[i].position.x, lights[i].position.y, lights[i].position.z);
		glUniform3f(ambLoc, lights[i].ambient.x, lights[i].ambient.y, lights[i].ambient.z);
		glUniform3f(diffLoc, lights[i].diffuse.x, lights[i].diffuse.y, lights[i].diffuse.z);
		glUniform3f(specLoc, lights[i].specular.x, lights[i].specular.y, lights[i].specular.z);
	}
	// Set the number of lights
	GLint numLightsLoc = glGetUniformLocation(shaderProgram, "numLights");
	glUniform1i(numLightsLoc, lightCount);
}

void Room::Draw(GLFWwindow* window, const Camera& camera)
{
	shader_wall.Activate();
	vao_wall.Bind();

	const int lightCount = 1;
	Light lights[lightCount];

	Light light;
	light.position = glm::vec3(0.0f, 0.0f, 0.0f);
	light.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	light.specular = glm::vec3(0.4f, 0.4f, 0.4f);

	lights[0] = light;
	loadLightUniform(shader_wall.ID, lights, lightCount);

	// Camera location
	GLint viewPos = glGetUniformLocation(shader_wall.ID, "viewPos");
	auto cameraPos = camera.GetPosition();
	glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);

	for (int i = 0; i < sizeof(walls_modelMtx) / sizeof(walls_modelMtx[0]); i++) {

		glUniformMatrix4fv(glGetUniformLocation(shader_wall.ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(walls_modelMtx[i]));
		camera.SaveMatrixToShader(shader_wall.ID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	vao_wall.Unbind();
}

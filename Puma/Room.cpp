#include "Room.h"
#include "EBO.h"

Room::Room(float size)
{
	std::vector<float> wallVs = { 
		-size * 0.5f, -size * 0.5f, 0,
		-size * 0.5f, size * 0.5f, 0,
		size * 0.5f, size * 0.5f, 0,
		size * 0.5f, -size * 0.5f, 0,

	};

	int halfSize = size * 0.5f;

	std::vector<GLuint> wallIes = {
		0, 1, 2,
		0, 2, 3
	};

	vao_wall.Bind();
	VBO vbo(wallVs, GL_STATIC_DRAW);
	EBO ebo(wallIes);

	vao_wall.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	vao_wall.Unbind(); vbo.Unbind(); ebo.Unbind();

	shader_wall = { "simple_vert.glsl", "simple_frag.glsl" };

	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0, 0.0, -halfSize));
	walls_modelMtx[0] = trans;

	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0, 0.0, +halfSize));
	walls_modelMtx[1] = trans;

	trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::translate(trans, glm::vec3(0.0, -halfSize, 0.0));
	walls_modelMtx[2] = trans;

	trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::translate(trans, glm::vec3(0.0, +halfSize, 0.0));
	walls_modelMtx[3] = trans;

	trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	trans = glm::translate(trans, glm::vec3(-halfSize, 0.0, 0.0));
	walls_modelMtx[4] = trans;

	trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	trans = glm::translate(trans, glm::vec3(+halfSize, 0.0, 0.0));
	walls_modelMtx[5] = trans;
}

void Room::Draw(GLFWwindow* window, const Camera& camera)
{
	shader_wall.Activate();
	vao_wall.Bind();
	for (int i = 0; i < walls_modelMtx->length(); i++) {
		glUniformMatrix4fv(glGetUniformLocation(shader_wall.ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(walls_modelMtx[i]));
		glUniform4f(glGetUniformLocation(shader_wall.ID, "COLOR"), wall_color.x, wall_color.y, wall_color.z, wall_color.w);
		camera.SaveMatrixToShader(shader_wall.ID);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	vao_wall.Unbind();
}

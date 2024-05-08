#include "Mirror.h"
#include "OpenGLHelper.h"

glm::mat4 Mirror::GetModelMtx()
{
	return mirror_modelMtx;
}

Mirror::Mirror(float width, float height, glm::vec3 pos)
{
	float halfWidth = width * 0.5f;
	float halfHeight = width * 0.5f;
	std::vector<float> wallVs = {
		-halfWidth, -halfHeight, 0, 0, 0, -1, mirror_color.r, mirror_color.g, mirror_color.b,
		-halfWidth, +halfHeight, 0, 0, 0, -1, mirror_color.r, mirror_color.g, mirror_color.b,
		+halfWidth, +halfHeight, 0, 0, 0, -1, mirror_color.r, mirror_color.g, mirror_color.b,
		+halfWidth, -halfHeight, 0, 0, 0, -1, mirror_color.r, mirror_color.g, mirror_color.b
	};

	std::vector<GLuint> wallIes = {
		0, 1, 2,
		0, 2, 3
	};

	vao_mirror.Bind();
	VBO vbo(wallVs, GL_STATIC_DRAW);
	EBO ebo(wallIes);

	vao_mirror.LinkAttrib(0, 3, GL_FLOAT, false, 9 * sizeof(float), 0);
	vao_mirror.LinkAttrib(1, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	vao_mirror.LinkAttrib(2, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)(6 * sizeof(float)));

	vao_mirror.Unbind(); vbo.Unbind(); ebo.Unbind();

	shader_mirror = { "phong_vert.glsl", "phong_frag.glsl" };

	auto trans = glm::mat4(1.0f);
	trans = glm::translate(trans, pos);
	trans = glm::rotate(trans, glm::radians(30.0f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	mirror_modelMtx = trans;
}

void Mirror::Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount)
{

	shader_mirror.Activate();
	vao_mirror.Bind();
	{
		OpenGLHelper::loadLightUniform(shader_mirror.ID, lights, lightsCount);

		// Camera location
		GLint viewPos = glGetUniformLocation(shader_mirror.ID, "viewPos");
		auto cameraPos = camera.GetPosition();
		glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);
		glUniformMatrix4fv(glGetUniformLocation(shader_mirror.ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(mirror_modelMtx));
		camera.SaveMatrixToShader(shader_mirror.ID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	vao_mirror.Unbind();
}

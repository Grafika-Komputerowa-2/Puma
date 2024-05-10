#include "Spark.h"
#include <chrono>

void Spark::Reset()
{
	lastPos = pos = startPos;
	birthTime = glfwGetTime();
	maxAge = ((float)rand() / (float)RAND_MAX) * 3;
	velocity = RandomStartDirection() * ((float)rand() / (float)RAND_MAX);
}

Spark::Spark()
{
	shader = StaticShaders::GetSimpleShader();
	std::vector<float> vs{};
	std::vector<GLuint> ids{};

	glm::vec3 startPos = { 2, 2, 2 };
	OpenGLHelper::AddVecToVector(vs, startPos);
	glm::vec3 sum{ 0, 0, 0 };
	for (int i = 1; i < 1001; i++)
	{
		glm::vec3 tmp = RandomStartDirection();
		sum += tmp;
		OpenGLHelper::AddVecToVector(vs, startPos + tmp * 2.0f);
		ids.push_back(0);
		ids.push_back(i);
	}
	sum /= 1000;



	Reset();
}

glm::vec3 Spark::RandomStartDirection()
{
	std::random_device rd{};
	std::mt19937 gen{ rd() };

	std::normal_distribution<float>  d{ 0.0f, 0.2f };

	float alfa1 = d(gen) * 180.0f;
	float alfa2 = d(gen) * 180.0f; 

	glm::vec3 start{ 0, 0, -1 };
	glm::mat4 trans{ 1.0f };
	trans = glm::rotate(trans, glm::radians(alfa1), glm::vec3(1, 0, 0));
	trans = glm::rotate(trans, glm::radians(alfa2), glm::vec3(0, 1, 0));

	auto result = glm::normalize(glm::vec3(trans * glm::vec4(start, 0)));
	
	return result;
}

void Spark::Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount, glm::mat4 trans)
{
	float leftTime = maxAge - glfwGetTime() + birthTime;
	if (leftTime < 0)
	{

		Reset();
		velocity = trans * glm::vec4{ velocity, 0 };
	}

	shader.Activate();
	vao.Bind();
	{
		OpenGLHelper::loadLightUniform(shader.ID, lights, lightsCount, trans);


		//glUniform1f(glGetUniformLocation(shader.ID, "alfa"), 1);
		// Camera location
		GLint viewPos = glGetUniformLocation(shader.ID, "viewPos");
		auto cameraPos = camera.GetPosition();
		glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(trans * glm::mat4{1.0f}));
		camera.SaveMatrixToShader(shader.ID);

		glUniform4f(glGetUniformLocation(shader.ID, "COLOR"),
			1, 1, 1, leftTime / maxAge);

		glDrawArrays(GL_LINES, 0, 2);
	}
	vao.Unbind();
}

void Spark::UpdatePos(float scalar)
{
	vao.Reactive();
	lastPos = pos;
	pos += velocity * scalar;

	std::vector<float> vs;
	OpenGLHelper::AddVecToVector(vs, lastPos);
	OpenGLHelper::AddVecToVector(vs, pos);

	vao.Bind();
	VBO vbo(vs, GL_DYNAMIC_DRAW);

	vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	vao.Unbind(); vbo.Unbind();
}

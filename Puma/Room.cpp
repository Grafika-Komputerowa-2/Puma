#include "Room.h"
#include "EBO.h"
#include "OpenGLHelper.h"



Room::Room(float width, float height, float deep)
{
	const float halfWidth = width * 0.5f;
	const float halfHeight = height * 0.5f;
	const float halfDeep = deep * 0.5f;

	// Location, Normal, Color
	std::vector<float> wallVs = { 
		-1, -1, 0, 0, 0, -1, 0.66f, 0.70f, 0.21f,
		-1, +1, 0, 0, 0, -1, 0.66f, 0.70f, 0.21f,
		+1, +1, 0, 0, 0, -1, 0.66f, 0.70f, 0.21f,
		+1, -1, 0, 0, 0, -1, 0.66f, 0.70f, 0.21f,
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

	shader = StaticShaders::GetPhongShader();

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
	trans = glm::rotate(trans, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
	trans = glm::scale(trans, glm::vec3(halfDeep, halfHeight, 1));
	walls_modelMtx[4] = trans;

	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(+halfWidth, 0.0, 0.0));
	trans = glm::translate(trans, glm::vec3(0.0, startPos_y, 0.0));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	trans = glm::scale(trans, glm::vec3(halfDeep, halfHeight, 1));
	walls_modelMtx[5] = trans;
}

void Room::DrawAll(GLFWwindow* window, const Camera& camera, Light* lights, int lightCount)
{

	if(animationOn)
		Animation();


	mirror.Reflect(window, camera, lights, lightCount, objectsToDraw, numberOfObjects);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // Pass test if stencil value is 1
	glStencilMask(0x00); // Don't write anything to stencil buffer
	glDepthMask(GL_TRUE); // Write to depth buffer
	for (int i = 0; i < numberOfObjects; i++) {
		objectsToDraw[i]->Draw(window, camera, lights, lightCount);
	}
	glDisable(GL_STENCIL_TEST);
	mirror.Draw(window, camera, lights, lightCount); // rysowanie pó³ przezroczystego lustra

	glDepthMask(GL_FALSE);
	if (animationOn) {
		sparkGenerator.Draw(window, camera, lights, lightCount);
	}
	glDepthMask(GL_TRUE);
}

void Room::Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount, glm::mat4 trans)

{
	shader.Activate();
	vao_wall.Bind();

	OpenGLHelper::loadLightUniform(shader.ID, lights, lightsCount, trans);
	glUniform1f(glGetUniformLocation(shader.ID, "alfa"), alfa);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Camera location
	GLint viewPos = glGetUniformLocation(shader.ID, "viewPos");
	auto cameraPos = camera.GetPosition();
	glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);

	for (int i = 0; i < sizeof(walls_modelMtx) / sizeof(walls_modelMtx[0]); i++) {

		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(trans * walls_modelMtx[i]));
		camera.SaveMatrixToShader(shader.ID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	vao_wall.Unbind();

	glDisable(GL_CULL_FACE);
}

void Room::Animation()
{
	const float size = 0.6f;
	glm::vec3 pos = Parametryzations::circle(glfwGetTime()) * size;
	glm::vec4 transformedPos = mirror.GetModelMtx() * glm::vec4{ pos, 1 };
	transformedPos /= transformedPos.w;

	glm::vec4 transformedNormal = mirror.GetModelMtx() * glm::vec4{ mirror.normal, 0 };

	robot.inverse_kinematics(transformedPos, transformedNormal);

	float scalar = glfwGetTime() - lastTime;
	sparkGenerator.Update(scalar, transformedPos, glm::vec3{ 0, -3, 0 });
	lastTime = glfwGetTime();
}

void Room::UserInterfers()
{
	ImGui::Begin("Interfers");
	{
		robot.UserInterfers();
		if (ImGui::RadioButton("Animation", animationOn))
			animationOn = !animationOn;

	}
	ImGui::End();
}

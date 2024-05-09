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

	shader = StaticShaders::GetPhongShader();
	shader_reflectionWord = { "reflectionWordPhong_vert.glsl", "reflectionWordPhong_frag.glsl" };

	auto trans = glm::mat4(1.0f);
	trans = glm::translate(trans, pos);
	trans = glm::rotate(trans, glm::radians(30.0f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::rotate(trans, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
	mirror_modelMtx = trans;

	alfa = 0.8f;
}

void Mirror::Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount)
{

	shader.Activate();
	vao_mirror.Bind();
	{
		OpenGLHelper::loadLightUniform(shader.ID, lights, lightsCount);

		glUniform1f(glGetUniformLocation(shader.ID, "alfa"), alfa);

		// Camera location
		GLint viewPos = glGetUniformLocation(shader.ID, "viewPos");
		auto cameraPos = camera.GetPosition();
		glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(mirror_modelMtx));
		camera.SaveMatrixToShader(shader.ID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	vao_mirror.Unbind();
}

void Mirror::Reflect(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount, ScensObject** sc, int n)
{

	glm::mat4 reflectionMtx = GetModelMtx() *
		glm::scale(glm::mat4{ 1.0f }, glm::vec3{ 1, 1, -1 }) *
		glm::inverse(GetModelMtx());

	glEnable(GL_STENCIL_TEST);


	// jedynki tam gdzie jest lustro
	glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF); // Write to stencil buffer
	glDepthMask(GL_FALSE); // Don't write to depth buffer
	glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

	Draw(window, camera, lights, lightsCount);

	// jeœli coœ zas³ania lustro wstaw zero
	glStencilFunc(GL_ALWAYS, 0, 0xFF); 
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	shader_reflectionWord.Activate();
	auto mirrorSide = glGetUniformLocation(shader_reflectionWord.ID, "mirrorSide");
	glUniform1f(mirrorSide, -1);
	for (int i = 0; i < n; i++) {
		Shader oldShader = sc[i]->SwapShaders(shader_reflectionWord);
		sc[i]->Draw(window, camera, lights, lightsCount);
		sc[i]->SwapShaders(oldShader);
	}

	// rysuj odbity œwiat tam gdzie jedynka
	glUniform1f(mirrorSide, 1); // mówi czy przed lustrem czy za

	glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
	glStencilMask(0x00); // Don't write anything to stencil buffer
	glDepthMask(GL_TRUE); // Write to depth buffer

	glUniformMatrix4fv(glGetUniformLocation(shader_reflectionWord.ID, "reflectPlaneModelMtx_invers"),
		1, GL_FALSE, glm::value_ptr(glm::inverse(mirror_modelMtx)));
	

	for (int i = 0; i < n; i++) {
		Shader oldShader = sc[i]->SwapShaders(shader_reflectionWord);
		sc[i]->Draw(window, camera, lights, lightsCount, reflectionMtx);
		sc[i]->SwapShaders(oldShader);
	}

	glDisable(GL_STENCIL_TEST);
}
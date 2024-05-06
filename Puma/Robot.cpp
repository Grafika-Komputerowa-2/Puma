#include "Robot.h"

void Robot::Draw(GLFWwindow* window, const Camera& camera)
{
	trianglesShader.Activate();
	for (int i = 0; i < 6; i++) {
		partsVAO_triangles[i].Bind(); 
		{
			glUniformMatrix4fv(glGetUniformLocation(trianglesShader.ID, "MODEL_MATRIX"),
				1, GL_FALSE, glm::value_ptr(partsModel_Mtx[i]));
			glUniform4f(glGetUniformLocation(trianglesShader.ID, "COLOR"), partsColors[i].x, partsColors[i].y, partsColors[i].z, partsColors[i].w);
			camera.SaveMatrixToShader(trianglesShader.ID);

			int n = partsInfo[i].triangles.size() /
				partsInfo[i].triangles_numberOfFloatsInDataUnit;
			glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
		}
		partsVAO_triangles[i].Unbind();
	}

	edgesShader.Activate();
	for (int i = 0; i < 6; i++) {
		partsVAO_edges[i].Bind();
		{
			glUniformMatrix4fv(glGetUniformLocation(edgesShader.ID, "MODEL_MATRIX"),
				1, GL_FALSE, glm::value_ptr(partsModel_Mtx[i]));
			glUniform4f(glGetUniformLocation(edgesShader.ID, "COLOR"), 0, 0, 0, 1);
			camera.SaveMatrixToShader(edgesShader.ID);

			int n = partsInfo[i].vertexPositions.size() /
				partsInfo[i].vertexPositions_numberOfFloatsInDataUnit;
			glDrawElements(GL_LINES, n, GL_UNSIGNED_INT, 0);
		}
		partsVAO_edges[i].Unbind();
	}
}

Robot::Robot()
{
	const char* meshSource[6]{
		".\\puma-mesh\\mesh1.txt",
		".\\puma-mesh\\mesh2.txt",
		".\\puma-mesh\\mesh3.txt",
		".\\puma-mesh\\mesh4.txt",
		".\\puma-mesh\\mesh5.txt",
		".\\puma-mesh\\mesh6.txt"
	};

	for (int i = 0; i < 6; i++) {
		partsInfo[i] = AszklarMeshReader::Read(meshSource[i]);
		partsModel_Mtx[i] = glm::mat4{ 1.0f };

		//////////////// triangle ///////////////////
		{
			std::vector<float> vertexAndNormals;
			int n = partsInfo[i].vertexId_And_normals.size() / partsInfo[i].vertexId_And_normals_numberOfFloatsInDataUnit;

			for (int j = 0; j < n; j++) {
				int vertexPosId = OpenGLHelper::TakeOneCopyOf<float>(partsInfo[i].vertexId_And_normals,
					j, 0, partsInfo[i].vertexId_And_normals_numberOfFloatsInDataUnit);
				glm::vec3 vertexPos = OpenGLHelper::TakeCopyOfVec3(partsInfo[i].vertexPositions,
					vertexPosId, 0, partsInfo[i].vertexPositions_numberOfFloatsInDataUnit);
				OpenGLHelper::AddVecToVector(vertexAndNormals, vertexPos);

				glm::vec3 normal = OpenGLHelper::TakeCopyOfVec3(partsInfo[i].vertexId_And_normals,
					j, 1, partsInfo[i].vertexId_And_normals_numberOfFloatsInDataUnit);
				OpenGLHelper::AddVecToVector(vertexAndNormals, normal);

			}

			partsVAO_triangles[i].Bind();
			VBO vbo(vertexAndNormals, GL_STATIC_DRAW);
			EBO ebo(partsInfo[i].triangles);

			partsVAO_triangles[i].LinkAttrib(0, 3, GL_FLOAT, false, 6 * sizeof(float), 0); // position
			partsVAO_triangles[i].LinkAttrib(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float))); // normals

			partsVAO_triangles[i].Unbind(); vbo.Unbind(); ebo.Unbind();
		}
		//////////////////////// edge ///////////
		{
			std::vector<GLuint> edgesId;

			int n = partsInfo[i].edges.size() / partsInfo[i].edges_numberOfFloatsInDataUnit;
			for (int j = 0; j < n; j++) {
				int vertexPosId = OpenGLHelper::TakeOneCopyOf<GLuint>(partsInfo[i].edges,
					j, 0, partsInfo[i].edges_numberOfFloatsInDataUnit);

				edgesId.push_back(vertexPosId);

				vertexPosId = OpenGLHelper::TakeOneCopyOf<GLuint>(partsInfo[i].edges,
					j, 1, partsInfo[i].edges_numberOfFloatsInDataUnit);

				edgesId.push_back(vertexPosId);
			}

			partsVAO_edges[i].Bind();
			VBO vbo(partsInfo[i].vertexPositions, GL_STATIC_DRAW);
			EBO ebo(edgesId);

			partsVAO_edges[i].LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0); // position

			partsVAO_edges[i].Unbind(); vbo.Unbind(); ebo.Unbind();
		}
	}

	trianglesShader = { "vertexAndNormals_vert.glsl", "vertexAndNormals_frag.glsl" };
	edgesShader = { "simple_vert.glsl", "simple_frag.glsl" };
}

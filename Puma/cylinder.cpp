#include "cylinder.h"

glm::vec3 Cylinder::CylinderVertex(float teta, float t)
{
	return { cosf(teta), sinf(teta), t };
}

Cylinder::Cylinder(float radius, float height, int sample, glm::vec3 pos, float yRotation)
{
    BuildVerticesSmooth(sample);
    auto trans = glm::mat4(1.0f);
    trans = glm::translate(trans, pos);
    trans = glm::rotate(trans, glm::radians(yRotation), glm::vec3{ 0, 1, 0 });
    trans = glm::scale(trans, glm::vec3(radius, radius, height * 0.5f));
    cilinder_modelMtx = trans;
}

void Cylinder::Draw(GLFWwindow* window, const Camera& camera, const Light* lights, int lightsCount, glm::mat4 trans)
{
    shader.Activate();
    vao_cilinder.Bind();
    {
        OpenGLHelper::loadLightUniform(shader.ID, lights, lightsCount, trans);
        glUniform1f(glGetUniformLocation(shader.ID, "alfa"), alfa);
        // Camera location
        GLint viewPos = glGetUniformLocation(shader.ID, "viewPos");
        auto cameraPos = camera.GetPosition();
        glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "MODEL_MATRIX"),
            1, GL_FALSE, glm::value_ptr(trans * cilinder_modelMtx));
        camera.SaveMatrixToShader(shader.ID);
        glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, 0);
    }
    vao_cilinder.Unbind();
}

std::vector<float> Cylinder::GetUnitCircleVertices(float sectorCount)
{
    const float PI = 3.1415926f;
    float sectorStep = 2 * PI / sectorCount;
    float sectorAngle;  // radian

    std::vector<float> unitCircleVertices;
    for (int i = 0; i <= sectorCount; ++i)
    {
        sectorAngle = i * sectorStep;
        unitCircleVertices.push_back(cos(sectorAngle)); // x
        unitCircleVertices.push_back(sin(sectorAngle)); // y
        unitCircleVertices.push_back(0);                // z
    }
    return unitCircleVertices;
}

void Cylinder::BuildVerticesSmooth(float sectorCount)
{
    // clear memory of prev arrays
    std::vector<float> verticesNormals;

    // get unit circle vectors on XY-plane
    std::vector<float> unitVertices = GetUnitCircleVertices(sectorCount);

    const float height = 1;
    const float radius = 1;

    // put side vertices to arrays
    for (int i = 0; i < 2; ++i)
    {
        float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
        float t = 1.0f - i;                              // vertical tex coord; 1 to 0

        for (int j = 0, k = 0; j <= sectorCount; ++j, k++)
        {
            auto u = OpenGLHelper::TakeCopyOfVec3(unitVertices, k, 0, 3);
            // position vector
            OpenGLHelper::AddVecToVector(verticesNormals, { u.x * radius , u.y * radius , h });
            // normal vector
            OpenGLHelper::AddVecToVector(verticesNormals, u);
            // color
            OpenGLHelper::AddVecToVector(verticesNormals, color);


        }
    }

    // the starting index for the base/top surface
    //NOTE: it is used for generating indices later
    int baseCenterIndex = (int)verticesNormals.size() / 9;
    int topCenterIndex = baseCenterIndex + sectorCount + 1; // include center vertex

    // put base and top vertices to arrays
    for (int i = 0; i < 2; ++i)
    {
        float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
        float nz = -1 + i * 2;                           // z value of normal; -1 to 1

        // center point
        {
            // position
            OpenGLHelper::AddVecToVector(verticesNormals, { 0, 0, h });
            // normal
            OpenGLHelper::AddVecToVector(verticesNormals, { 0, 0, nz });
            // color
            OpenGLHelper::AddVecToVector(verticesNormals, color);
        }

       
        for (int j = 0, k = 0; j < sectorCount; ++j, k += 3)
        {
            float ux = unitVertices[k];
            float uy = unitVertices[k + 1];
            // position vector
            OpenGLHelper::AddVecToVector(verticesNormals, { ux * radius, uy * radius, h });
            // normal vector
            OpenGLHelper::AddVecToVector(verticesNormals, { 0, 0, nz });
            // color
            OpenGLHelper::AddVecToVector(verticesNormals, color);
        }
    }

    // generate CCW index list of cylinder triangles
    std::vector<GLuint> indices;
    int k1 = 0;                         // 1st vertex index at base
    int k2 = sectorCount + 1;           // 1st vertex index at top

    // indices for the side surface
    for (int i = 0; i < sectorCount; ++i, ++k1, ++k2)
    {
        // 2 triangles per sector
        // k1 => k1+1 => k2
        OpenGLHelper::AddVecToVector(indices, { k1, k1 + 1, k2 });

        // k2 => k1+1 => k2+1
        OpenGLHelper::AddVecToVector(indices, { k2, k1 + 1, k2 + 1});
    }

    // indices for the base surface
    //NOTE: baseCenterIndex and topCenterIndices are pre-computed during vertex generation
    //      please see the previous code snippet
    for (int i = 0, k = baseCenterIndex + 1; i < sectorCount; ++i, ++k)
    {
        if (i < sectorCount - 1)
        {
            OpenGLHelper::AddVecToVector(indices, { baseCenterIndex, k + 1, k });
        }
        else // last triangle
        {
            OpenGLHelper::AddVecToVector(indices, { baseCenterIndex, baseCenterIndex + 1, k });
        }
    }

    // indices for the top surface
    for (int i = 0, k = topCenterIndex + 1; i < sectorCount; ++i, ++k)
    {
        if (i < sectorCount - 1)
        {
            OpenGLHelper::AddVecToVector(indices, { topCenterIndex, k, k + 1 });
        }
        else // last triangle
        {
            OpenGLHelper::AddVecToVector(indices, { topCenterIndex, k, topCenterIndex + 1 });
        }
    }

    numberOfIndices = indices.size();
    vao_cilinder.Bind();
    VBO vbo(verticesNormals, GL_STATIC_DRAW);
    EBO ebo(indices);

    vao_cilinder.LinkAttrib(0, 3, GL_FLOAT, false, 9 * sizeof(float), 0);
    vao_cilinder.LinkAttrib(1, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    vao_cilinder.LinkAttrib(2, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)(6 * sizeof(float)));

    vao_cilinder.Unbind(); vbo.Unbind(); ebo.Unbind();

    shader = StaticShaders::GetPhongShader();
}
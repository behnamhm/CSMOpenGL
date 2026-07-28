#include "meshObject.h"
#include <cmath>
#include <GLFW\glfw3.h>
#include <glm/gtc/quaternion.hpp>

Mesh::Mesh()
{
    int vCount = 1;
    float radius = 1.0;
    GLuint m_VBO = 0;
    GLuint m_EBO = 0;
    GLuint m_VAO = 0;
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;
}

void Mesh::Create(const std::vector<float>& vertices, const std::vector<uint32_t>& indices)
{
    this->indices = indices;
    this->vertices = vertices;
    m_VBO = CreateVertexBuffer(vertices);
    m_EBO = CreateIndexBuffer(indices);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);




    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Mesh::DrawObject(std::string& ObjName)
{
    if (ObjName == "Floor")
    {
        DrawFloor();
    }
    else if (ObjName == "Cube")
    {
        DrawCube();
    }
}


void Mesh::DrawFloor()
{
    std::vector<unsigned int> indices = {
        0, 2, 1,
        1, 2, 3
    };

    std::vector<float> vertices = {
        -12.0f, 0.0f, -12.0f,	0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
        12.0f, 0.0f, -12.0f,	0.0f, -1.0f, 0.0f,   3.0f, 0.0f,
        -12.0f, 0.0f, 12.0f,	0.0f, -1.0f, 0.0f,   0.0f, 3.0f,
        12.0f, 0.0f, 12.0f,	    0.0f, -1.0f, 0.0f,   3.0f, 3.0f
    };
    Create(vertices, indices);

}

void Mesh::DrawCube()
{

    std::vector<unsigned int> skyboxIndices = {
        // front
        0, 1, 2,
        2, 1, 3,
        // right
        2, 3, 5,
        5, 3, 7,
        // back
        5, 7, 4,
        4, 7, 6,
        // left
        4, 6, 0,
        0, 6, 1,
        // top
        4, 0, 5,
        5, 0, 2,
        // bottom
        1, 6, 3,
        3, 6, 7
    };

    std::vector<float> skyboxVertices = {
        -1.0f, 1.0f, -1.0f,		0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,	0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
        1.0f, 1.0f, -1.0f,		0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,		0.0f, -1.0f, 0.0f,   0.0f, 0.0f,

        -1.0f, 1.0f, 1.0f,		0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,		0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,		0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,		0.0f, -1.0f, 0.0f,   0.0f, 0.0f
    };
    Create(skyboxVertices, skyboxIndices);

}

void Mesh::DrawQuad()
{

    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}



std::vector <float> Mesh::GetVertices() const
{
    return vertices;
}

std::vector<unsigned int> Mesh::GetIndices() const
{
    return indices;
}

std::vector<GLfloat> Mesh::GetNormals() const
{
    return normals;
}


int Mesh::GetVCount()
{
    return vCount;
}

float Mesh::GetRadius()
{
    return radius;
}

void Mesh::Render()
{
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


GLuint Mesh::CreateVertexBuffer(const std::vector<float>& vertices)
{
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return VBO;
}

GLuint Mesh::CreateIndexBuffer(const std::vector<uint32_t>& indices)
{
    GLuint EBO = 0;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return EBO;
}


void Mesh::keyControl(
    bool* keys,
    GLfloat deltaTime,
    glm::vec3& sphere_pos,
    glm::quat& sphere_rot,
    float radius,
    const glm::vec3& camFront,
    const glm::vec3& camRight)
{
    glm::vec3 movement(0.0f);
    float speed = 4.0f;
    if (keys[GLFW_KEY_W]) movement += camFront;
    if (keys[GLFW_KEY_S]) movement -= camFront;
    if (keys[GLFW_KEY_D]) movement += camRight;
    if (keys[GLFW_KEY_A]) movement -= camRight;
    if (keys[GLFW_KEY_SPACE]) speed += 4.0;

    movement.y = 0.0f;

    if (glm::length(movement) == 0.0f)
        return;

    movement = glm::normalize(movement);


    glm::vec3 displacement = movement * speed * deltaTime;

    sphere_pos += displacement;

    float distance = glm::length(displacement);
    float angle = distance / radius;

    glm::vec3 axis = glm::cross(glm::vec3(0, 1, 0), movement);

    if (glm::length(axis) > 0.0001f)
        axis = glm::normalize(axis);
    else
        return;

    glm::quat deltaRot = glm::angleAxis(angle, axis);

    sphere_rot = glm::normalize(deltaRot * sphere_rot);
}


Mesh::~Mesh() {}


#pragma once

#include <memory>
#include <string>

#include <vector>
#include <glm/glm.hpp> 
#include "vertexLayout.h"
#include "objectTypes.h"



class Mesh
{
public:
    Mesh();
    void Create(const std::vector<float>& vertices, const std::vector<uint32_t>& indices);
    void keyControl(bool* keys, GLfloat deltaTime, glm::vec3& sphere_pos, glm::quat& sphere_rot,
        float radius, const glm::vec3& camFront, const glm::vec3& camRight);
    void DrawObject(std::string& ObjName);
    void DrawFloor();
    void DrawQuad();
    void DrawCube();

    void Render();

    GLuint CreateVertexBuffer(const std::vector<float>& vertices);
    GLuint CreateIndexBuffer(const std::vector<uint32_t>& indices);
    std::vector<float> GetVertices() const;
    std::vector<unsigned int> GetIndices() const;
    std::vector<GLfloat> GetNormals() const;
    int GetVCount();
    float GetRadius();
    int vCount;
    float radius;
    ~Mesh();

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;

    GLuint m_VBO = 0;
    GLuint m_EBO = 0;
    GLuint m_VAO = 0;
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

    uint32_t stride = 0;
};
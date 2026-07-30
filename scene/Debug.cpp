#include "Debug.h"
#include "components/shader.h"



void Debug::drawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader)
{
    visualizerVAOs.resize(8);
    visualizerEBOs.resize(8);
    visualizerVBOs.resize(8);

    const GLuint indices[] = {
        0, 2, 3,
        0, 3, 1,
        4, 6, 2,
        4, 2, 0,
        5, 7, 6,
        5, 6, 4,
        1, 3, 7,
        1, 7, 5,
        6, 7, 3,
        6, 3, 2,
        1, 5, 4,
        0, 1, 4
    };

    const glm::vec4 colors[] = {
        {1.0, 0.0, 0.0, 0.5f},
        {0.0, 1.0, 0.0, 0.5f},
        {0.0, 0.0, 1.0, 0.5f},
    };

    for (int i = 0; i < lightMatrices.size(); ++i)
    {
        const auto corners = shadowSystem.getFrustumCornersWorldSpace(lightMatrices[i]);
        std::vector<glm::vec3> vec3s;
        for (const auto& v : corners)
        {
            vec3s.push_back(glm::vec3(v));
        }

        glGenVertexArrays(1, &visualizerVAOs[i]);
        glGenBuffers(1, &visualizerVBOs[i]);
        glGenBuffers(1, &visualizerEBOs[i]);

        glBindVertexArray(visualizerVAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, visualizerVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizerEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glBindVertexArray(visualizerVAOs[i]);
        shader->setUniform("color", colors[i % 3]);
        glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);

        glDeleteBuffers(1, &visualizerVBOs[i]);
        glDeleteBuffers(1, &visualizerEBOs[i]);
        glDeleteVertexArrays(1, &visualizerVAOs[i]);

        glBindVertexArray(0);
    }

    visualizerVAOs.clear();
    visualizerEBOs.clear();
    visualizerVBOs.clear();
}


void Debug::drawCascadeflatVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader)
{
    flatvisualizerVAOs.resize(8);
    flatvisualizerEBOs.resize(8);
    flatvisualizerVBOs.resize(8);

    const GLuint indices[] =
    {
        0, 1, 2,
        1, 2, 3
    };

    const glm::vec4 colors[] = {
        {1.0, 0.0, 0.0, 0.5f},
        {0.0, 1.0, 0.0, 0.5f},
        {0.0, 0.0, 1.0, 0.5f},
    };

    for (int i = static_cast<int>(lightMatrices.size()) - 1; i >= 0; --i)
    {
        const auto corners = getFrustumFlatCornersWorldSpace(lightMatrices[i]);
        std::vector<glm::vec3> vec3s;
        const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50.0f, 20.0f));

        for (const auto& v : corners) 
        { 
        if (v.y < 0.0f) 
            continue;  
        glm::vec3 p(v); float t = -p.y / lightDir.y; 
        glm::vec3 projected = p + t * lightDir; 


        vec3s.push_back(projected); 
        }

        glGenVertexArrays(1, &flatvisualizerVAOs[i]);
        glGenBuffers(1, &flatvisualizerVBOs[i]);
        glGenBuffers(1, &flatvisualizerEBOs[i]);

        glBindVertexArray(flatvisualizerVAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, flatvisualizerVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, flatvisualizerEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glStencilFunc(GL_EQUAL, 0, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0xFF);

        glBindVertexArray(flatvisualizerVAOs[i]);
        shader->setUniform("color", colors[i % 3]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLES, GLsizei(6), GL_UNSIGNED_INT, 0);
        glDeleteBuffers(1, &flatvisualizerVBOs[i]);
        glDeleteBuffers(1, &flatvisualizerEBOs[i]);
        glDeleteVertexArrays(1, &flatvisualizerVAOs[i]);

        glBindVertexArray(0);
    }

    flatvisualizerVAOs.clear();
    flatvisualizerEBOs.clear();
    flatvisualizerVBOs.clear();
}


std::vector<glm::vec4> Debug::getFrustumFlatCornersWorldSpace(const glm::mat4& projview)
{
    const auto inv = glm::inverse(projview);

    std::vector<glm::vec4> corners;

    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                glm::vec4 pt = inv * glm::vec4(
                    2.0f * x - 1.0f,
                    2.0f * y - 1.0f,
                    2.0f * z - 1.0f,
                    1.0f);

                corners.push_back(pt / pt.w);
            }
        }
    }

    std::sort(corners.begin(), corners.end(),
        [](const glm::vec4& a, const glm::vec4& b)
        {
            return a.y > b.y;
        });

    corners.resize(4);

    return corners;
}
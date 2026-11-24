#ifndef CROSSING_H
#define CROSSING_H

#include <glm/glm.hpp>
#include <vector>

class Shader;

class Crossing {
public:
    // Each lane is given as (z of its centre, driving direction)
    Crossing(const glm::vec3& center, float halfWidth, const std::vector<glm::vec2>& lanes);
    ~Crossing();

    Crossing(const Crossing&) = delete;
    Crossing& operator=(const Crossing&) = delete;

    void setGeometry(const glm::vec3& center, float halfWidth);
    bool stopsCar(const glm::vec3& carPosition, float carDirection, float carHalfLength, bool signalActive) const;
    void render(const Shader& shader) const;
    void renderStonePile(const Shader& shader, int stoneCount, const glm::vec3& pileCenter) const;

private:
    void uploadMesh();
    void uploadStonePile();

    glm::vec3 position;
    float halfWidth;
    std::vector<glm::vec2> lanes;

    // OpenGL variables
    unsigned int VAO, VBO;
    int vertexCount;

    // Stone pile rendering data
    unsigned int stonePileVAO, stonePileVBO;
    int stonePileVertexCount;
};

#endif // CROSSING_H

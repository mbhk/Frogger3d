#include "Ground.h"
#include "MeshUtil.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace {

struct Slab {
    glm::vec3 center;
    glm::vec3 size;
    glm::vec3 color;
};

const float groundWidth = 30.0f;

const Slab slabs[] = {
    {glm::vec3(0.0f, -0.1f, 2.5f),   glm::vec3(groundWidth, 0.2f, 1.0f), glm::vec3(0.6f, 0.6f, 0.6f)},    // lower pavement
    {glm::vec3(0.0f, -0.1f, 0.0f),   glm::vec3(groundWidth, 0.2f, 4.0f), glm::vec3(0.2f, 0.2f, 0.22f)},   // road
    {glm::vec3(0.0f, -0.1f, -2.5f),  glm::vec3(groundWidth, 0.2f, 1.0f), glm::vec3(0.6f, 0.6f, 0.6f)},    // upper pavement
    {glm::vec3(0.0f, 0.005f, 1.0f),  glm::vec3(groundWidth, 0.02f, 0.08f), glm::vec3(0.9f, 0.9f, 0.9f)},  // lane divider
    {glm::vec3(0.0f, 0.005f, 0.0f),  glm::vec3(groundWidth, 0.02f, 0.08f), glm::vec3(0.9f, 0.9f, 0.9f)},  // lane divider
    {glm::vec3(0.0f, 0.005f, -1.0f), glm::vec3(groundWidth, 0.02f, 0.08f), glm::vec3(0.9f, 0.9f, 0.9f)},  // lane divider
};

}

Ground::Ground() : VAO(0), VBO(0), vertexCount(0) {
    std::vector<float> mesh;
    for (const auto& slab : slabs) {
        appendBox(mesh, slab.center, slab.size, slab.color);
    }
    vertexCount = static_cast<int>(mesh.size() / 6);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Ground::~Ground() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Ground::render(const Shader& shader) const {
    shader.setMat4("uModel", glm::mat4(1.0f));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

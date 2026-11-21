#include "Lamp.h"
#include "MeshUtil.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace {

const float poleHeight = 1.6f;
const glm::vec3 poleSize(0.1f, poleHeight, 0.1f);
const glm::vec3 poleColor(0.25f, 0.25f, 0.28f);
const glm::vec3 headSize(0.3f, 0.12f, 0.3f);
const glm::vec3 headColor(1.0f, 0.95f, 0.6f);
const glm::vec3 lightColor(1.0f, 0.9f, 0.6f);

}

Lamp::Lamp(const glm::vec3& base) : position(base), lightPosition(base + glm::vec3(0.0f, poleHeight, 0.0f)), VAO(0), VBO(0), headOffset(0), vertexCount(0) {
    std::vector<float> mesh;
    appendBox(mesh, glm::vec3(0.0f, poleHeight * 0.5f, 0.0f), poleSize, poleColor);
    headOffset = static_cast<int>(mesh.size() / 6);
    appendBox(mesh, glm::vec3(0.0f, poleHeight, 0.0f), headSize, headColor);
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

Lamp::~Lamp() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Lamp::render(const Shader& shader) const {
    shader.setMat4("uModel", glm::translate(glm::mat4(1.0f), position));

    glBindVertexArray(VAO);
    shader.setInt("uEmissive", 0);
    glDrawArrays(GL_TRIANGLES, 0, headOffset);
    shader.setInt("uEmissive", 1);
    glDrawArrays(GL_TRIANGLES, headOffset, vertexCount - headOffset);
    shader.setInt("uEmissive", 0);
    glBindVertexArray(0);
}

void Lamp::renderShadow(const Shader& shader, const glm::mat4& shadowProjection) const {
    shader.setMat4("uModel", shadowProjection * glm::translate(glm::mat4(1.0f), position));
    shader.setInt("uShadow", 1);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    shader.setInt("uShadow", 0);
}

PointLight Lamp::getLight() const {
    return {lightPosition, lightColor};
}

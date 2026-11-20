#include "Enemy.h"
#include "MeshUtil.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace {

struct CarPart {
    glm::vec3 center;
    glm::vec3 size;
    glm::vec3 color;
};

const glm::vec3 bodySize(1.6f, 0.5f, 0.9f);

const CarPart carParts[] = {
    {glm::vec3(-0.1f, 0.45f, 0.0f),  glm::vec3(0.8f, 0.4f, 0.8f),  glm::vec3(0.7f, 0.8f, 0.9f)},  // cabin
    {glm::vec3(0.5f, -0.25f, 0.5f),  glm::vec3(0.3f, 0.3f, 0.1f),  glm::vec3(0.1f, 0.1f, 0.1f)},  // wheels
    {glm::vec3(-0.5f, -0.25f, 0.5f), glm::vec3(0.3f, 0.3f, 0.1f),  glm::vec3(0.1f, 0.1f, 0.1f)},
    {glm::vec3(0.5f, -0.25f, -0.5f), glm::vec3(0.3f, 0.3f, 0.1f),  glm::vec3(0.1f, 0.1f, 0.1f)},
    {glm::vec3(-0.5f, -0.25f, -0.5f), glm::vec3(0.3f, 0.3f, 0.1f), glm::vec3(0.1f, 0.1f, 0.1f)},
};

}

Enemy::Enemy(float x, float y, float z, const glm::vec3& bodyColor)
    : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), speed(3.0f), direction(1.0f), vertexCount(0) {

    std::vector<float> mesh;
    appendBox(mesh, glm::vec3(0.0f), bodySize, bodyColor);
    for (const auto& part : carParts) {
        appendBox(mesh, part.center, part.size, part.color);
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

Enemy::~Enemy() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Enemy::update(float deltaTime) {
    // Simple AI - move back and forth
    velocity.x = direction * speed;
    
    // Update position
    position += velocity * deltaTime;
    
    // Simple boundary checking to reverse direction
    if (position.x > 5.0f || position.x < -5.0f) {
        direction *= -1.0f;
    }
}

void Enemy::render(const Shader& shader) const {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    shader.setMat4("uModel", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

void Enemy::move(float dx, float dy, float dz) {
    velocity.x = dx * speed;
    velocity.y = dy * speed;
    velocity.z = dz * speed;
}

glm::vec3 Enemy::getPosition() const {
    return position;
}

glm::vec3 Enemy::getHalfExtents() const {
    // Body and wheels; the wheel-to-body overhang is kept out so cars in neighbouring lanes don't touch
    return glm::vec3(0.8f, 0.3f, 0.45f);
}

void Enemy::setPosition(float x, float y, float z) {
    position.x = x;
    position.y = y;
    position.z = z;
}

#include "Enemy.h"
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

void appendBox(std::vector<float>& out, const glm::vec3& center, const glm::vec3& size, const glm::vec3& color) {
    const glm::vec3 h = size * 0.5f;
    const glm::vec3 corners[8] = {
        center + glm::vec3(-h.x, -h.y, -h.z),
        center + glm::vec3( h.x, -h.y, -h.z),
        center + glm::vec3( h.x,  h.y, -h.z),
        center + glm::vec3(-h.x,  h.y, -h.z),
        center + glm::vec3(-h.x, -h.y,  h.z),
        center + glm::vec3( h.x, -h.y,  h.z),
        center + glm::vec3( h.x,  h.y,  h.z),
        center + glm::vec3(-h.x,  h.y,  h.z),
    };
    const int triangles[12][3] = {
        {4, 5, 6}, {4, 6, 7},  // front
        {1, 0, 3}, {1, 3, 2},  // back
        {5, 1, 2}, {5, 2, 6},  // right
        {0, 4, 7}, {0, 7, 3},  // left
        {7, 6, 2}, {7, 2, 3},  // top
        {0, 1, 5}, {0, 5, 4},  // bottom
    };
    for (const auto& triangle : triangles) {
        for (int index : triangle) {
            const glm::vec3& p = corners[index];
            out.insert(out.end(), {p.x, p.y, p.z, color.r, color.g, color.b});
        }
    }
}

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

void Enemy::setPosition(float x, float y, float z) {
    position.x = x;
    position.y = y;
    position.z = z;
}

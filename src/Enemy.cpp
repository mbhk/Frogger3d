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
const float exitLimit = 8.0f;
const glm::vec3 headlightColor(1.0f, 0.95f, 0.6f);
const glm::vec3 taillightColor(0.9f, 0.05f, 0.05f);

// Local space: the hood points along +X
const CarPart carParts[] = {
    {glm::vec3(-0.2f, 0.45f, 0.0f),  glm::vec3(0.8f, 0.4f, 0.8f),  glm::vec3(0.7f, 0.8f, 0.9f)},  // cabin
    {glm::vec3(0.82f, -0.05f, 0.28f),  glm::vec3(0.04f, 0.12f, 0.16f), headlightColor},  // headlight
    {glm::vec3(0.82f, -0.05f, -0.28f), glm::vec3(0.04f, 0.12f, 0.16f), headlightColor},  // headlight
    {glm::vec3(-0.82f, -0.05f, 0.28f), glm::vec3(0.04f, 0.12f, 0.16f), taillightColor},  // taillight
    {glm::vec3(-0.82f, -0.05f, -0.28f), glm::vec3(0.04f, 0.12f, 0.16f), taillightColor},  // taillight
    {glm::vec3(0.5f, -0.25f, 0.5f),  glm::vec3(0.3f, 0.3f, 0.1f),  glm::vec3(0.1f, 0.1f, 0.1f)},  // wheels
    {glm::vec3(-0.5f, -0.25f, 0.5f), glm::vec3(0.3f, 0.3f, 0.1f),  glm::vec3(0.1f, 0.1f, 0.1f)},
    {glm::vec3(0.5f, -0.25f, -0.5f), glm::vec3(0.3f, 0.3f, 0.1f),  glm::vec3(0.1f, 0.1f, 0.1f)},
    {glm::vec3(-0.5f, -0.25f, -0.5f), glm::vec3(0.3f, 0.3f, 0.1f), glm::vec3(0.1f, 0.1f, 0.1f)},
};

struct CarLight {
    glm::vec3 center;
    glm::vec3 color;
};

const CarLight carLights[] = {
    {glm::vec3(0.82f, -0.05f, 0.28f), headlightColor},
    {glm::vec3(0.82f, -0.05f, -0.28f), headlightColor},
    {glm::vec3(-0.82f, -0.05f, 0.28f), taillightColor},
    {glm::vec3(-0.82f, -0.05f, -0.28f), taillightColor},
};

}

Enemy::Enemy(float x, float y, float z, const glm::vec3& bodyColor, float direction)
    : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), speed(3.0f), direction(direction), vertexCount(0) {

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
    // Drive along the lane in its direction
    velocity.x = direction * speed;
    
    // Update position
    position += velocity * deltaTime;
}

bool Enemy::isPastExit() const {
    return direction > 0.0f ? position.x > exitLimit : position.x < -exitLimit;
}

void Enemy::setSpeed(float value) {
    speed = value;
}

void Enemy::render(const Shader& shader) const {
    shader.setMat4("uModel", modelMatrix());

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

std::vector<PointLight> Enemy::getLights() const {
    const glm::mat4 model = modelMatrix();
    std::vector<PointLight> lights;
    for (const auto& light : carLights) {
        lights.push_back({glm::vec3(model * glm::vec4(light.center, 1.0f)), light.color});
    }
    return lights;
}

glm::mat4 Enemy::modelMatrix() const {
    const float yaw = direction > 0.0f ? 0.0f : glm::radians(180.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    return glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
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

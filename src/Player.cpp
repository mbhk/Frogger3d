#include "Player.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

namespace {

// Local space: the frog faces -Z (the goal), with its feet on y = 0
struct FrogPart {
    glm::vec3 center;
    glm::vec3 radii;
    glm::vec3 color;
};

const glm::vec3 skinColor(0.2f, 0.72f, 0.2f);
const glm::vec3 darkSkinColor(0.12f, 0.5f, 0.14f);
const glm::vec3 eyeColor(0.95f, 0.95f, 0.85f);
const glm::vec3 pupilColor(0.05f, 0.05f, 0.05f);

const FrogPart frogParts[] = {
    {glm::vec3(0.0f, 0.22f, 0.02f),   glm::vec3(0.30f, 0.20f, 0.34f), skinColor},      // body
    {glm::vec3(0.0f, 0.24f, -0.22f),  glm::vec3(0.26f, 0.17f, 0.18f), skinColor},      // head
    {glm::vec3(-0.26f, 0.14f, 0.20f), glm::vec3(0.16f, 0.15f, 0.20f), darkSkinColor},  // left haunch
    {glm::vec3(0.26f, 0.14f, 0.20f),  glm::vec3(0.16f, 0.15f, 0.20f), darkSkinColor},  // right haunch
    {glm::vec3(-0.30f, 0.04f, 0.32f), glm::vec3(0.12f, 0.04f, 0.18f), darkSkinColor},  // left back foot
    {glm::vec3(0.30f, 0.04f, 0.32f),  glm::vec3(0.12f, 0.04f, 0.18f), darkSkinColor},  // right back foot
    {glm::vec3(-0.24f, 0.12f, -0.18f), glm::vec3(0.08f, 0.07f, 0.10f), skinColor},     // left front arm
    {glm::vec3(0.24f, 0.12f, -0.18f),  glm::vec3(0.08f, 0.07f, 0.10f), skinColor},     // right front arm
    {glm::vec3(-0.27f, 0.03f, -0.27f), glm::vec3(0.07f, 0.03f, 0.08f), darkSkinColor}, // left front foot
    {glm::vec3(0.27f, 0.03f, -0.27f),  glm::vec3(0.07f, 0.03f, 0.08f), darkSkinColor}, // right front foot
    {glm::vec3(-0.20f, 0.42f, -0.30f), glm::vec3(0.10f), eyeColor},                    // left eye
    {glm::vec3(0.20f, 0.42f, -0.30f),  glm::vec3(0.10f), eyeColor},                    // right eye
    {glm::vec3(-0.21f, 0.43f, -0.39f), glm::vec3(0.05f), pupilColor},                  // left pupil
    {glm::vec3(0.21f, 0.43f, -0.39f),  glm::vec3(0.05f), pupilColor},                  // right pupil
};

void appendEllipsoid(std::vector<float>& mesh, const FrogPart& part) {
    const int stacks = 8;
    const int slices = 12;
    auto point = [&](int stack, int slice) {
        const float phi = glm::pi<float>() * stack / stacks;
        const float theta = glm::two_pi<float>() * slice / slices;
        const glm::vec3 unit(std::sin(phi) * std::cos(theta), std::cos(phi), std::sin(phi) * std::sin(theta));
        return part.center + part.radii * unit;
    };
    auto push = [&](const glm::vec3& p) {
        mesh.insert(mesh.end(), {p.x, p.y, p.z, part.color.r, part.color.g, part.color.b});
    };
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            push(point(stack, slice));
            push(point(stack + 1, slice));
            push(point(stack + 1, slice + 1));
            push(point(stack, slice));
            push(point(stack + 1, slice + 1));
            push(point(stack, slice + 1));
        }
    }
}

}

Player::Player(float x, float y, float z) 
    : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), speed(5.0f), vertexCount(0) {

    std::vector<float> mesh;
    for (const auto& part : frogParts) {
        appendEllipsoid(mesh, part);
    }
    vertexCount = static_cast<int>(mesh.size() / 6);
    
    // Initialize OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

Player::~Player() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Player::update(float deltaTime) {
    // Simple update logic - in a real game this would handle input and physics
    position += velocity * deltaTime;
    
    // Reset velocity
    velocity = glm::vec3(0.0f);
}

void Player::render(const Shader& shader) const {
    shader.setMat4("uModel", glm::translate(glm::mat4(1.0f), position));

    // Glossy skin: the frog catches light highlights
    shader.setFloat("uSpecularStrength", 0.8f);
    shader.setFloat("uShininess", 32.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    shader.setFloat("uSpecularStrength", 0.0f);
}

void Player::move(float dx, float dy, float dz) {
    velocity.x = dx * speed;
    velocity.y = dy * speed;
    velocity.z = dz * speed;
}

glm::vec3 Player::getPosition() const {
    return position;
}

void Player::setPosition(float x, float y, float z) {
    position.x = x;
    position.y = y;
    position.z = z;
}

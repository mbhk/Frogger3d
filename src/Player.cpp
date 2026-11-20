#include "Player.h"
#include "FrogData.h"
#include "MeshUtil.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iterator>
#include <vector>

namespace {

// Local space: the eyes sit on the front face (normal (1, 1, -1)), which is turned to face -Z (the goal)
const float frontYaw = glm::radians(45.0f);

struct FrogPart {
    glm::vec3 center;
    glm::vec3 size;
    glm::vec3 color;
};

const FrogPart frogEyes[] = {
    {glm::vec3(0.27f, 0.10f, -0.19f), glm::vec3(0.12f), glm::vec3(1.0f, 1.0f, 1.0f)},
    {glm::vec3(0.10f, 0.27f, -0.19f), glm::vec3(0.12f), glm::vec3(1.0f, 1.0f, 1.0f)},
};

}

Player::Player(float x, float y, float z) 
    : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), speed(5.0f), vertexCount(0) {

    std::vector<float> mesh(std::begin(frogVertices), std::end(frogVertices));
    for (const auto& eye : frogEyes) {
        appendBox(mesh, eye.center, eye.size, eye.color);
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
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::rotate(model, frontYaw, glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("uModel", model);

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

#include "Player.h"
#include "CubeData.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

Player::Player(float x, float y, float z) 
    : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), speed(5.0f) {
    
    // Initialize OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    
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
    glDeleteBuffers(1, &EBO);
}

void Player::update(float deltaTime) {
    // Simple update logic - in a real game this would handle input and physics
    position += velocity * deltaTime;
    
    // Reset velocity
    velocity = glm::vec3(0.0f);
}

void Player::render(const Shader& shader) const {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    shader.setMat4("uModel", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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

#include "Player.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Player::Player(float x, float y, float z) 
    : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), speed(5.0f) {}

Player::~Player() = default;

void Player::update(float deltaTime) {
    // Simple update logic - in a real game this would handle input and physics
    position += velocity * deltaTime;
    
    // Reset velocity
    velocity = glm::vec3(0.0f);
}

void Player::render() const {
    // Simple cube representation for the player
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    
    // Draw a simple cube
    glBegin(GL_QUADS);
    
    // Front face
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    
    // Back face
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    
    // Top face
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    
    // Bottom face
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    
    glEnd();
    
    glPopMatrix();
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
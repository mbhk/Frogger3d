#include "Enemy.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Enemy::Enemy(float x, float y, float z) 
    : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), speed(3.0f), direction(1.0f) {}

Enemy::~Enemy() = default;

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

void Enemy::render() const {
    // Simple cube representation for the enemy
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    
    // Draw a simple cube
    glBegin(GL_QUADS);
    
    // Front face
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    
    // Back face
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    
    // Top face
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    
    // Bottom face
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    
    glEnd();
    
    glPopMatrix();
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
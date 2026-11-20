#ifndef ENEMY_H
#define ENEMY_H

#include <glm/glm.hpp>
#include <memory>

class Shader;

class Enemy {
public:
    Enemy(float x, float y, float z);
    ~Enemy();

    void update(float deltaTime);
    void render(const Shader& shader) const;
    void move(float dx, float dy, float dz);
    glm::vec3 getPosition() const;
    void setPosition(float x, float y, float z);

private:
    glm::vec3 position;
    glm::vec3 velocity;
    float speed;
    float direction;

    // OpenGL variables
    unsigned int VAO, VBO, EBO;
};

#endif // ENEMY_H

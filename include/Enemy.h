#ifndef ENEMY_H
#define ENEMY_H

#include <glm/glm.hpp>
#include <memory>

class Shader;

class Enemy {
public:
    Enemy(float x, float y, float z, const glm::vec3& bodyColor, float direction);
    ~Enemy();

    void update(float deltaTime);
    bool isPastExit() const;
    void setSpeed(float value);
    void render(const Shader& shader) const;
    void move(float dx, float dy, float dz);
    glm::vec3 getPosition() const;
    glm::vec3 getHalfExtents() const;
    void setPosition(float x, float y, float z);

private:
    glm::vec3 position;
    glm::vec3 velocity;
    float speed;
    float direction;

    // OpenGL variables
    unsigned int VAO, VBO;
    int vertexCount;
};

#endif // ENEMY_H

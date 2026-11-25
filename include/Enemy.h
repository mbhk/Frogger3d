#ifndef ENEMY_H
#define ENEMY_H

#include "Light.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Shader;

enum class CarType {
    NORMAL,
    SPORTS,
    HAULER
};

class Enemy {
public:
    Enemy(float x, float y, float z, const glm::vec3& bodyColor, float direction, CarType carType = CarType::NORMAL);
    ~Enemy();

    void update(float deltaTime);
    bool isPastExit() const;
    void setSpeed(float value);
    void render(const Shader& shader) const;
    void renderShadow(const Shader& shader, const glm::mat4& shadowProjection) const;
    void move(float dx, float dy, float dz);
    glm::vec3 getPosition() const;
    glm::vec3 getHalfExtents() const;
    void setPosition(float x, float y, float z);
    std::vector<PointLight> getLights() const;
    CarType getCarType() const;

private:
    glm::mat4 modelMatrix() const;

    glm::vec3 position;
    glm::vec3 velocity;
    float speed;
    float direction;
    CarType carType;
    glm::vec3 bodySize;
    float lightX;

    // OpenGL variables
    unsigned int VAO, VBO;
    int emissiveOffset;
    int vertexCount;
};

#endif // ENEMY_H

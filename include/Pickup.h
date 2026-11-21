#ifndef PICKUP_H
#define PICKUP_H

#include <glm/glm.hpp>

class Shader;

class Pickup {
public:
    Pickup(float x, float y, float z, float lifetime);
    ~Pickup();

    Pickup(const Pickup&) = delete;
    Pickup& operator=(const Pickup&) = delete;

    void update(float deltaTime);
    bool isExpired() const;
    void render(const Shader& shader) const;
    void renderShadow(const Shader& shader, const glm::mat4& shadowProjection) const;
    glm::vec3 getPosition() const;
    glm::vec3 getHalfExtents() const;

private:
    glm::mat4 modelMatrix() const;

    glm::vec3 position;
    float age;
    float lifetime;

    // OpenGL variables
    unsigned int VAO, VBO;
    int vertexCount;
};

#endif // PICKUP_H

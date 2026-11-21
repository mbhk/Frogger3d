#ifndef LAMP_H
#define LAMP_H

#include "Light.h"
#include <glm/glm.hpp>

class Shader;

class Lamp {
public:
    explicit Lamp(const glm::vec3& base);
    ~Lamp();

    Lamp(const Lamp&) = delete;
    Lamp& operator=(const Lamp&) = delete;

    void render(const Shader& shader) const;
    void renderShadow(const Shader& shader, const glm::mat4& shadowProjection) const;
    PointLight getLight() const;

private:
    glm::vec3 position;
    glm::vec3 lightPosition;

    // OpenGL variables
    unsigned int VAO, VBO;
    int headOffset;
    int vertexCount;
};

#endif // LAMP_H

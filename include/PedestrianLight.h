#ifndef PEDESTRIAN_LIGHT_H
#define PEDESTRIAN_LIGHT_H

#include <glm/glm.hpp>

class Shader;

class PedestrianLight {
public:
    explicit PedestrianLight(const glm::vec3& base);
    ~PedestrianLight();

    PedestrianLight(const PedestrianLight&) = delete;
    PedestrianLight& operator=(const PedestrianLight&) = delete;

    void update(float deltaTime);
    bool press();
    void reset();
    void setPosition(const glm::vec3& base);
    bool blocksTraffic() const;
    void render(const Shader& shader) const;
    void renderShadow(const Shader& shader, const glm::mat4& shadowProjection) const;

private:
    enum class Phase { Off, Red, Green };

    glm::vec3 position;
    Phase phase;
    float phaseTimer;

    // OpenGL variables
    unsigned int VAO, VBO;
    int bodyVertexCount;
    int vertexCount;
};

#endif // PEDESTRIAN_LIGHT_H

#ifndef GROUND_H
#define GROUND_H

#include <glm/glm.hpp>

class Shader;

class Ground {
public:
    Ground();
    ~Ground();

    Ground(const Ground&) = delete;
    Ground& operator=(const Ground&) = delete;

    void render(const Shader& shader, bool corn) const;
    void renderCornShadow(const Shader& shader, const glm::mat4& shadowProjection) const;

private:
    // OpenGL variables
    unsigned int VAO, VBO;
    int sharedVertexCount;
    int grassVertexCount;
    int cornVertexCount;
};

#endif // GROUND_H

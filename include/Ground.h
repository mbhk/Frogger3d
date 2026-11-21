#ifndef GROUND_H
#define GROUND_H

class Shader;

class Ground {
public:
    Ground();
    ~Ground();

    Ground(const Ground&) = delete;
    Ground& operator=(const Ground&) = delete;

    void render(const Shader& shader, bool corn) const;

private:
    // OpenGL variables
    unsigned int VAO, VBO;
    int sharedVertexCount;
    int grassVertexCount;
    int cornVertexCount;
};

#endif // GROUND_H

#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <memory>

class Shader;

class Player {
public:
    Player(float x, float y, float z);
    ~Player();

    void update(float deltaTime);
    void render(const Shader& shader) const;
    void move(float dx, float dy, float dz);
    glm::vec3 getPosition() const;
    void setPosition(float x, float y, float z);
    void hopTo(float x, float y, float z);

private:
    glm::vec3 position;
    glm::vec3 velocity;
    float speed;

    bool hopping;
    glm::vec3 hopStart;
    glm::vec3 hopEnd;
    float hopElapsed;
    
    // OpenGL variables
    unsigned int VAO, VBO;
    int vertexCount;
};

#endif // PLAYER_H

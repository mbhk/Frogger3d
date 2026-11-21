#include "Pickup.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

namespace {

const int outlineSegments = 96;
const float heartScale = 0.025f;
const float heartHalfDepth = 0.06f;
const glm::vec3 heartColor(1.0f, 0.15f, 0.25f);
const float bobHeight = 0.08f;
const float bobSpeed = 3.0f;
const float spinSpeed = 2.5f;
const glm::vec3 heartHalfExtents(0.4f, 0.3f, 0.4f);

// Classic parametric heart curve, scaled and centred on the origin
glm::vec2 heartOutline(float t) {
    const float x = 16.0f * std::pow(std::sin(t), 3.0f);
    const float y = 13.0f * std::cos(t) - 5.0f * std::cos(2.0f * t) - 2.0f * std::cos(3.0f * t) - std::cos(4.0f * t);
    return glm::vec2(x, y) * heartScale;
}

void pushVertex(std::vector<float>& mesh, const glm::vec3& p) {
    mesh.insert(mesh.end(), {p.x, p.y, p.z, heartColor.r, heartColor.g, heartColor.b});
}

}

Pickup::Pickup(float x, float y, float z, float lifetime)
    : position(x, y, z), age(0.0f), lifetime(lifetime), VAO(0), VBO(0), vertexCount(0) {

    // Flat heart slab: front and back faces are fans from the centre, the rim is the side wall
    std::vector<glm::vec2> outline;
    for (int i = 0; i < outlineSegments; ++i) {
        outline.push_back(heartOutline(glm::two_pi<float>() * i / outlineSegments));
    }

    std::vector<float> mesh;
    for (int i = 0; i < outlineSegments; ++i) {
        const glm::vec2& a = outline[i];
        const glm::vec2& b = outline[(i + 1) % outlineSegments];
        const glm::vec3 aFront(a.x, a.y, heartHalfDepth), bFront(b.x, b.y, heartHalfDepth);
        const glm::vec3 aBack(a.x, a.y, -heartHalfDepth), bBack(b.x, b.y, -heartHalfDepth);

        pushVertex(mesh, glm::vec3(0.0f, 0.0f, heartHalfDepth));
        pushVertex(mesh, aFront);
        pushVertex(mesh, bFront);

        pushVertex(mesh, glm::vec3(0.0f, 0.0f, -heartHalfDepth));
        pushVertex(mesh, bBack);
        pushVertex(mesh, aBack);

        pushVertex(mesh, aFront);
        pushVertex(mesh, bFront);
        pushVertex(mesh, bBack);
        pushVertex(mesh, aFront);
        pushVertex(mesh, bBack);
        pushVertex(mesh, aBack);
    }
    vertexCount = static_cast<int>(mesh.size() / 6);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Pickup::~Pickup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Pickup::update(float deltaTime) {
    age += deltaTime;
}

bool Pickup::isExpired() const {
    return age >= lifetime;
}

void Pickup::render(const Shader& shader) const {
    const float bob = bobHeight * std::sin(age * bobSpeed);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position + glm::vec3(0.0f, bob, 0.0f));
    model = glm::rotate(model, age * spinSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("uModel", model);

    shader.setFloat("uSpecularStrength", 0.8f);
    shader.setFloat("uShininess", 32.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    shader.setFloat("uSpecularStrength", 0.0f);
}

glm::vec3 Pickup::getPosition() const {
    return position;
}

glm::vec3 Pickup::getHalfExtents() const {
    return heartHalfExtents;
}

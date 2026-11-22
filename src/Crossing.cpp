#include "Crossing.h"
#include "MeshUtil.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

namespace {

const float stopLineGap = 1.0f;
const float stopLineThickness = 0.1f;
const float laneWidth = 0.9f;
const float stripeWidth = 0.25f;
const float stripeSpacing = 0.5f;
const int stripeCount = 8;
const float markingY = 0.01f;
const float markingHeight = 0.01f;
const glm::vec3 markingColor(0.9f, 0.9f, 0.9f);

}

Crossing::Crossing(const glm::vec3& center, float halfWidth, const std::vector<glm::vec2>& lanes)
    : position(center), halfWidth(halfWidth), lanes(lanes), VAO(0), VBO(0), vertexCount(0) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    uploadMesh();
}

Crossing::~Crossing() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Crossing::setGeometry(const glm::vec3& center, float newHalfWidth) {
    position = center;
    if (newHalfWidth != halfWidth) {
        halfWidth = newHalfWidth;
        uploadMesh();
    }
}

// Zebra stripes run along the traffic direction and are spaced across the road; each lane gets a stop line on the side the cars come from
void Crossing::uploadMesh() {
    std::vector<float> mesh;
    for (int i = 0; i < stripeCount; ++i) {
        const float z = (static_cast<float>(i) - (stripeCount - 1) * 0.5f) * stripeSpacing;
        appendBox(mesh, glm::vec3(0.0f, markingY, z), glm::vec3(2.0f * halfWidth, markingHeight, stripeWidth), markingColor);
    }
    for (const auto& lane : lanes) {
        const float side = lane.y > 0.0f ? -1.0f : 1.0f;
        const float x = side * (halfWidth + stopLineGap);
        appendBox(mesh, glm::vec3(x, markingY, lane.x), glm::vec3(stopLineThickness, markingHeight, laneWidth), markingColor);
    }
    vertexCount = static_cast<int>(mesh.size() / 6);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(), GL_STATIC_DRAW);
}

// Cars stop at the stop line while the signal is active, but cars already on the crossing drive on
bool Crossing::stopsCar(const glm::vec3& carPosition, float carDirection, float carHalfLength, bool signalActive) const {
    if (!signalActive) {
        return false;
    }
    const float front = carPosition.x + carDirection * carHalfLength;
    const float gap = carDirection > 0.0f
        ? (position.x - halfWidth) - front
        : front - (position.x + halfWidth);
    return gap >= 0.0f && gap < stopLineGap;
}

void Crossing::render(const Shader& shader) const {
    shader.setMat4("uModel", glm::translate(glm::mat4(1.0f), position));

    glBindVertexArray(VAO);
    shader.setInt("uEmissive", 0);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

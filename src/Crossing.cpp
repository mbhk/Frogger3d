#include "Crossing.h"
#include "MeshUtil.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

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
const int stonePileLayers = 3;
const int boxVertexCount = 36;
const glm::vec3 stoneCenters[stonePileLayers] = {
    glm::vec3(0.0f, 0.10f, 0.0f),
    glm::vec3(0.03f, 0.30f, 0.02f),
    glm::vec3(-0.02f, 0.48f, 0.0f),
};
const glm::vec3 stoneSizes[stonePileLayers] = {
    glm::vec3(0.5f, 0.2f, 0.4f),
    glm::vec3(0.36f, 0.2f, 0.3f),
    glm::vec3(0.22f, 0.16f, 0.2f),
};
const glm::vec3 stoneColors[stonePileLayers] = {
    glm::vec3(0.50f, 0.50f, 0.52f),
    glm::vec3(0.42f, 0.42f, 0.44f),
    glm::vec3(0.36f, 0.36f, 0.38f),
};

}

Crossing::Crossing(const glm::vec3& center, float halfWidth, const std::vector<glm::vec2>& lanes)
    : position(center), halfWidth(halfWidth), lanes(lanes), VAO(0), VBO(0), vertexCount(0), stonePileVAO(0), stonePileVBO(0), stonePileVertexCount(0) {
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

    glGenVertexArrays(1, &stonePileVAO);
    glGenBuffers(1, &stonePileVBO);

    glBindVertexArray(stonePileVAO);

    glBindBuffer(GL_ARRAY_BUFFER, stonePileVBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    uploadStonePile();
}

Crossing::~Crossing() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &stonePileVAO);
    glDeleteBuffers(1, &stonePileVBO);
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

// The pile is a fixed three-layer stack; renderStonePile draws only as many layers as stones collected
void Crossing::uploadStonePile() {
    std::vector<float> mesh;
    for (int i = 0; i < stonePileLayers; ++i) {
        appendBox(mesh, stoneCenters[i], stoneSizes[i], stoneColors[i]);
    }
    stonePileVertexCount = static_cast<int>(mesh.size() / 6);

    glBindBuffer(GL_ARRAY_BUFFER, stonePileVBO);
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

void Crossing::renderStonePile(const Shader& shader, int stoneCount, const glm::vec3& pileCenter) const {
    const int visibleLayers = std::min(stoneCount, stonePileLayers);
    if (visibleLayers <= 0) {
        return;
    }

    shader.setMat4("uModel", glm::translate(glm::mat4(1.0f), pileCenter));

    glBindVertexArray(stonePileVAO);
    shader.setInt("uEmissive", 0);
    glDrawArrays(GL_TRIANGLES, 0, visibleLayers * boxVertexCount);
    glBindVertexArray(0);
}

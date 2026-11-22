#include "PedestrianLight.h"
#include "MeshUtil.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace {

const float poleHeight = 1.6f;
const glm::vec3 poleSize(0.08f, poleHeight, 0.08f);
const glm::vec3 poleColor(0.25f, 0.25f, 0.28f);
const glm::vec3 housingSize(0.28f, 0.5f, 0.18f);
const float housingCenterY = poleHeight + 0.15f;
const glm::vec3 housingColor(0.15f, 0.15f, 0.16f);
const glm::vec3 lensSize(0.18f, 0.16f, 0.03f);
const float lensOffsetY = 0.12f;
const glm::vec3 redOffColor(0.25f, 0.05f, 0.04f);
const glm::vec3 redOnColor(1.0f, 0.15f, 0.1f);
const glm::vec3 greenOffColor(0.05f, 0.2f, 0.07f);
const glm::vec3 greenOnColor(0.2f, 1.0f, 0.3f);
const glm::vec3 buttonSize(0.06f, 0.1f, 0.06f);
const glm::vec3 buttonColor(0.9f, 0.75f, 0.2f);
const float buttonY = 1.1f;
const float redDuration = 2.0f;
const float greenDuration = 8.0f;
const int boxVertexCount = 36;

}

PedestrianLight::PedestrianLight(const glm::vec3& base) : position(base), phase(Phase::Off), phaseTimer(0.0f), VAO(0), VBO(0), bodyVertexCount(0), vertexCount(0) {
    // Body first, then the red lens in off and on colour, then the green lens in off and on colour
    std::vector<float> mesh;
    appendBox(mesh, glm::vec3(0.0f, poleHeight * 0.5f, 0.0f), poleSize, poleColor);
    appendBox(mesh, glm::vec3(0.0f, housingCenterY, 0.0f), housingSize, housingColor);
    appendBox(mesh, glm::vec3(0.0f, buttonY, poleSize.z * 0.5f + buttonSize.z * 0.5f), buttonSize, buttonColor);
    bodyVertexCount = static_cast<int>(mesh.size() / 6);

    const glm::vec3 redPosition(0.0f, housingCenterY + lensOffsetY, housingSize.z * 0.5f);
    const glm::vec3 greenPosition(0.0f, housingCenterY - lensOffsetY, housingSize.z * 0.5f);
    appendBox(mesh, redPosition, lensSize, redOffColor);
    appendBox(mesh, redPosition, lensSize, redOnColor);
    appendBox(mesh, greenPosition, lensSize, greenOffColor);
    appendBox(mesh, greenPosition, lensSize, greenOnColor);
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

PedestrianLight::~PedestrianLight() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void PedestrianLight::update(float deltaTime) {
    if (phase == Phase::Off) {
        return;
    }
    phaseTimer -= deltaTime;
    if (phaseTimer > 0.0f) {
        return;
    }
    if (phase == Phase::Red) {
        phase = Phase::Green;
        phaseTimer += greenDuration;
    } else {
        phase = Phase::Off;
    }
}

bool PedestrianLight::press() {
    if (phase != Phase::Off) {
        return false;
    }
    phase = Phase::Red;
    phaseTimer = redDuration;
    return true;
}

void PedestrianLight::reset() {
    phase = Phase::Off;
    phaseTimer = 0.0f;
}

void PedestrianLight::setPosition(const glm::vec3& base) {
    position = base;
}

bool PedestrianLight::blocksTraffic() const {
    return phase != Phase::Off;
}

void PedestrianLight::render(const Shader& shader) const {
    shader.setMat4("uModel", glm::translate(glm::mat4(1.0f), position));

    glBindVertexArray(VAO);
    shader.setInt("uEmissive", 0);
    glDrawArrays(GL_TRIANGLES, 0, bodyVertexCount);

    shader.setInt("uEmissive", 1);
    const int redStart = bodyVertexCount + (phase == Phase::Red ? boxVertexCount : 0);
    const int greenStart = bodyVertexCount + 2 * boxVertexCount + (phase == Phase::Green ? boxVertexCount : 0);
    glDrawArrays(GL_TRIANGLES, redStart, boxVertexCount);
    glDrawArrays(GL_TRIANGLES, greenStart, boxVertexCount);
    shader.setInt("uEmissive", 0);
    glBindVertexArray(0);
}

void PedestrianLight::renderShadow(const Shader& shader, const glm::mat4& shadowProjection) const {
    shader.setMat4("uModel", shadowProjection * glm::translate(glm::mat4(1.0f), position));
    shader.setInt("uShadow", 1);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    shader.setInt("uShadow", 0);
}

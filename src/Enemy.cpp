#include "Enemy.h"
#include "MeshUtil.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

namespace {

struct CarPart {
    glm::vec3 center;
    glm::vec3 size;
    glm::vec3 color;
};

// Cars are removed once they pass this X on either side
const float exitLimit = 8.0f;
const glm::vec3 headlightColor(1.0f, 1.0f, 0.8f);
const glm::vec3 taillightColor(1.0f, 0.1f, 0.1f);
// Size of each light lens (x is thickness, y and z are the lens face)
const glm::vec3 lightSize(0.06f, 0.16f, 0.22f);
// Distance from the body side to the centre of a light lens
const float lightInset = 0.03f;
// Wheels reach 0.4 below the car's origin; this lifts them onto the road surface
const float carLift = 0.4f;
// Number of sides on each wheel cylinder and each arch half-ring
const float wheelSegments = 16.0f;
const float archSegments = 8.0f;
// Gap between the body side and the wheel arch, so the arch doesn't z-fight with the body
const float archInset = 0.005f;
// Radii of the wheel arch ring; the wheel itself is 0.15 in radius
const float archInnerRadius = 0.16f;
const float archOuterRadius = 0.23f;
const glm::vec3 archColor(0.05f, 0.05f, 0.05f);
const glm::vec3 hubColor(0.6f, 0.6f, 0.65f);

// Cabin colour per car type; the body colour is randomised in Game::spawnCar
const glm::vec3 normalColor(0.7f, 0.8f, 0.9f);
const glm::vec3 sportsColor(0.9f, 0.2f, 0.2f);
const glm::vec3 haulerColor(0.8f, 0.8f, 0.2f);

// Body dimensions per car type: x is length (along the direction of travel), z is width
// Keep width at or below 0.9 so cars stay inside their 1.0-unit lane between the lines
const glm::vec3 normalBodySize(1.6f, 0.5f, 0.9f);
const glm::vec3 sportsBodySize(1.8f, 0.4f, 0.8f);
const glm::vec3 haulerBodySize(2.2f, 0.6f, 0.8f);

// Local space: the hood points along +X
const CarPart normalParts[] = {
    {glm::vec3(-0.2f, 0.45f, 0.0f),  glm::vec3(0.8f, 0.4f, 0.8f),  normalColor},  // cabin
    {glm::vec3(0.5f, -0.25f, 0.41f),  glm::vec3(0.3f, 0.3f, 0.08f),  glm::vec3(0.1f, 0.1f, 0.1f)},  // wheels
    {glm::vec3(-0.5f, -0.25f, 0.41f), glm::vec3(0.3f, 0.3f, 0.08f),  glm::vec3(0.1f, 0.1f, 0.1f)},
    {glm::vec3(0.5f, -0.25f, -0.41f), glm::vec3(0.3f, 0.3f, 0.08f),  glm::vec3(0.1f, 0.1f, 0.1f)},
    {glm::vec3(-0.5f, -0.25f, -0.41f), glm::vec3(0.3f, 0.3f, 0.08f), glm::vec3(0.1f, 0.1f, 0.1f)},
};

const CarPart sportsParts[] = {
    {glm::vec3(0.0f, 0.4f, 0.0f),  glm::vec3(0.8f, 0.4f, 0.8f),  sportsColor},  // cabin
    {glm::vec3(0.8f, -0.25f, 0.36f),  glm::vec3(0.3f, 0.3f, 0.08f),  glm::vec3(0.1f, 0.1f, 0.1f)},  // wheels
    {glm::vec3(-0.8f, -0.25f, 0.36f), glm::vec3(0.3f, 0.3f, 0.08f),  glm::vec3(0.1f, 0.1f, 0.1f)},
    {glm::vec3(0.8f, -0.25f, -0.36f), glm::vec3(0.3f, 0.3f, 0.08f),  glm::vec3(0.1f, 0.1f, 0.1f)},
    {glm::vec3(-0.8f, -0.25f, -0.36f), glm::vec3(0.3f, 0.3f, 0.08f), glm::vec3(0.1f, 0.1f, 0.1f)},
};

const CarPart haulerParts[] = {
    {glm::vec3(-0.3f, 0.5f, 0.0f),  glm::vec3(1.0f, 0.5f, 0.8f),  haulerColor},  // cabin
    {glm::vec3(1.0f, -0.25f, 0.36f),  glm::vec3(0.3f, 0.3f, 0.08f),  glm::vec3(0.1f, 0.1f, 0.1f)},  // wheels
    {glm::vec3(-1.0f, -0.25f, 0.36f), glm::vec3(0.3f, 0.3f, 0.08f),  glm::vec3(0.1f, 0.1f, 0.1f)},
    {glm::vec3(1.0f, -0.25f, -0.36f), glm::vec3(0.3f, 0.3f, 0.08f),  glm::vec3(0.1f, 0.1f, 0.1f)},
    {glm::vec3(-1.0f, -0.25f, -0.36f), glm::vec3(0.3f, 0.3f, 0.08f), glm::vec3(0.1f, 0.1f, 0.1f)},
};

// Helper function to get car parts and body size based on car type
const CarPart* getCarParts(CarType carType, glm::vec3& bodySizeOut) {
    switch (carType) {
        case CarType::NORMAL:
            bodySizeOut = normalBodySize;
            return normalParts;
        case CarType::SPORTS:
            bodySizeOut = sportsBodySize;
            return sportsParts;
        case CarType::HAULER:
            bodySizeOut = haulerBodySize;
            return haulerParts;
    }
    bodySizeOut = normalBodySize;
    return normalParts;
}

void pushVertex(std::vector<float>& out, const glm::vec3& p, const glm::vec3& color) {
    out.insert(out.end(), {p.x, p.y, p.z, color.r, color.g, color.b});
}

// Cylinder with its axis along Z, so it reads as a wheel seen from the side
void appendCylinder(std::vector<float>& out, const glm::vec3& center, float radius, float width, const glm::vec3& color) {
    const glm::vec3 front(0.0f, 0.0f, width * 0.5f);
    const glm::vec3 back = -front;
    for (int i = 0; i < static_cast<int>(wheelSegments); ++i) {
        const float a0 = glm::two_pi<float>() * i / wheelSegments;
        const float a1 = glm::two_pi<float>() * (i + 1) / wheelSegments;
        const glm::vec3 r0(std::cos(a0) * radius, std::sin(a0) * radius, 0.0f);
        const glm::vec3 r1(std::cos(a1) * radius, std::sin(a1) * radius, 0.0f);
        pushVertex(out, center + front, color);
        pushVertex(out, center + front + r0, color);
        pushVertex(out, center + front + r1, color);
        pushVertex(out, center + back, color);
        pushVertex(out, center + back + r1, color);
        pushVertex(out, center + back + r0, color);
        pushVertex(out, center + front + r0, color);
        pushVertex(out, center + back + r0, color);
        pushVertex(out, center + back + r1, color);
        pushVertex(out, center + front + r0, color);
        pushVertex(out, center + back + r1, color);
        pushVertex(out, center + front + r1, color);
    }
}

void appendWheel(std::vector<float>& out, const glm::vec3& center, float radius, float width, const glm::vec3& tireColor) {
    appendCylinder(out, center, radius, width, tireColor);
    appendCylinder(out, center, radius * 0.45f, width + 0.01f, hubColor);
}

// Upper half-ring in the XY plane, sitting on the body side above a wheel
void appendArch(std::vector<float>& out, const glm::vec3& center, float innerRadius, float outerRadius, const glm::vec3& color) {
    for (int i = 0; i < static_cast<int>(archSegments); ++i) {
        const float a0 = glm::pi<float>() * i / archSegments;
        const float a1 = glm::pi<float>() * (i + 1) / archSegments;
        const glm::vec3 in0 = center + glm::vec3(std::cos(a0) * innerRadius, std::sin(a0) * innerRadius, 0.0f);
        const glm::vec3 in1 = center + glm::vec3(std::cos(a1) * innerRadius, std::sin(a1) * innerRadius, 0.0f);
        const glm::vec3 out0 = center + glm::vec3(std::cos(a0) * outerRadius, std::sin(a0) * outerRadius, 0.0f);
        const glm::vec3 out1 = center + glm::vec3(std::cos(a1) * outerRadius, std::sin(a1) * outerRadius, 0.0f);
        pushVertex(out, in0, color);
        pushVertex(out, out0, color);
        pushVertex(out, out1, color);
        pushVertex(out, in0, color);
        pushVertex(out, out1, color);
        pushVertex(out, in1, color);
    }
}

}

Enemy::Enemy(float x, float y, float z, const glm::vec3& bodyColor, float direction, CarType carType)
    : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), speed(3.0f), direction(direction), carType(carType), emissiveOffset(0), vertexCount(0) {

    const CarPart* parts = getCarParts(carType, bodySize);
    lightX = bodySize.x * 0.5f + lightInset;

    std::vector<float> mesh;
    appendBox(mesh, glm::vec3(0.0f), bodySize, bodyColor);
    appendBox(mesh, parts[0].center, parts[0].size, parts[0].color);
    for (int i = 1; i < 5; ++i) {
        const CarPart& wheel = parts[i];
        const float side = wheel.center.z > 0.0f ? 1.0f : -1.0f;
        appendWheel(mesh, wheel.center, wheel.size.y * 0.5f, wheel.size.z, wheel.color);
        appendArch(mesh, glm::vec3(wheel.center.x, wheel.center.y, side * (bodySize.z * 0.5f + archInset)),
                   archInnerRadius, archOuterRadius, archColor);
    }
    emissiveOffset = static_cast<int>(mesh.size() / 6);
    for (float side : {1.0f, -1.0f}) {
        for (float lane : {0.28f, -0.28f}) {
            appendBox(mesh, glm::vec3(side * lightX, -0.05f, lane), lightSize, side > 0.0f ? headlightColor : taillightColor);
        }
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

Enemy::~Enemy() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Enemy::update(float deltaTime) {
    // Drive along the lane in its direction
    velocity.x = direction * speed;
    
    // Update position
    position += velocity * deltaTime;
}

bool Enemy::isPastExit() const {
    return direction > 0.0f ? position.x > exitLimit : position.x < -exitLimit;
}

void Enemy::setSpeed(float value) {
    speed = value;
}

void Enemy::render(const Shader& shader) const {
    shader.setMat4("uModel", modelMatrix());

    glBindVertexArray(VAO);
    shader.setInt("uEmissive", 0);
    glDrawArrays(GL_TRIANGLES, 0, emissiveOffset);
    shader.setInt("uEmissive", 1);
    glDrawArrays(GL_TRIANGLES, emissiveOffset, vertexCount - emissiveOffset);
    shader.setInt("uEmissive", 0);
    glBindVertexArray(0);
}

void Enemy::renderShadow(const Shader& shader, const glm::mat4& shadowProjection) const {
    shader.setMat4("uModel", shadowProjection * modelMatrix());
    shader.setInt("uShadow", 1);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    shader.setInt("uShadow", 0);
}

std::vector<PointLight> Enemy::getLights() const {
    const glm::mat4 model = modelMatrix();
    std::vector<PointLight> lights;
    for (float side : {1.0f, -1.0f}) {
        for (float lane : {0.28f, -0.28f}) {
            const glm::vec3 center(side * lightX, -0.05f, lane);
            lights.push_back({glm::vec3(model * glm::vec4(center, 1.0f)), side > 0.0f ? headlightColor : taillightColor});
        }
    }
    return lights;
}

glm::mat4 Enemy::modelMatrix() const {
    const float yaw = direction > 0.0f ? 0.0f : glm::radians(180.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position + glm::vec3(0.0f, carLift, 0.0f));
    return glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Enemy::move(float dx, float dy, float dz) {
    velocity.x = dx * speed;
    velocity.y = dy * speed;
    velocity.z = dz * speed;
}

glm::vec3 Enemy::getPosition() const {
    return position;
}

glm::vec3 Enemy::getHalfExtents() const {
    // Half the body length per type; width and height are kept out of the wheels so cars in neighbouring lanes don't touch
    return glm::vec3(bodySize.x * 0.5f, 0.3f, 0.45f);
}

void Enemy::setPosition(float x, float y, float z) {
    position.x = x;
    position.y = y;
    position.z = z;
}

CarType Enemy::getCarType() const {
    return carType;
}

#include "Ground.h"
#include "MeshUtil.h"
#include "Shader.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <random>
#include <vector>

namespace {

struct Slab {
    glm::vec3 center;
    glm::vec3 size;
    glm::vec3 color;
};

const float groundWidth = 30.0f;
const float playgroundHalfDepth = 3.0f;
const float grassSurfaceY = -0.2f;
const float grassBaseSize = 160.0f;
const float fieldHalfWidth = 30.0f;
const float fieldHalfDepth = 20.0f;
const float bladeWidth = 0.05f;
const int clumpCount = 3500;
const float clumpRadius = 0.2f;
const glm::vec3 grassColor(0.25f, 0.55f, 0.2f);
const float cornSpacing = 0.6f;
const float cornStalkWidth = 0.05f;
const float cornLeafLength = 0.22f;
const glm::vec3 cornStalkColor(0.45f, 0.6f, 0.18f);
const glm::vec3 cornLeafColor(0.3f, 0.7f, 0.2f);
const glm::vec3 cornTasselColor(0.9f, 0.75f, 0.2f);

const Slab slabs[] = {
    {glm::vec3(0.0f, -0.1f, 2.5f),   glm::vec3(groundWidth, 0.2f, 1.0f), glm::vec3(0.6f, 0.6f, 0.6f)},    // lower pavement
    {glm::vec3(0.0f, -0.1f, 0.0f),   glm::vec3(groundWidth, 0.2f, 4.0f), glm::vec3(0.2f, 0.2f, 0.22f)},   // road
    {glm::vec3(0.0f, -0.1f, -2.5f),  glm::vec3(groundWidth, 0.2f, 1.0f), glm::vec3(0.6f, 0.6f, 0.6f)},    // upper pavement
    {glm::vec3(0.0f, 0.005f, 1.0f),  glm::vec3(groundWidth, 0.02f, 0.08f), glm::vec3(0.9f, 0.9f, 0.9f)},  // lane divider
    {glm::vec3(0.0f, 0.005f, 0.0f),  glm::vec3(groundWidth, 0.02f, 0.08f), glm::vec3(0.9f, 0.9f, 0.9f)},  // lane divider
    {glm::vec3(0.0f, 0.005f, -1.0f), glm::vec3(groundWidth, 0.02f, 0.08f), glm::vec3(0.9f, 0.9f, 0.9f)},  // lane divider
};

bool insidePlayground(float x, float z) {
    return std::abs(z) < playgroundHalfDepth && std::abs(x) < groundWidth * 0.5f;
}

float randomBetween(std::mt19937& engine, float low, float high) {
    std::uniform_real_distribution<float> distribution(low, high);
    return distribution(engine);
}

// Appends a tapered, curved blade (position + color) with a darker base and lighter tip
void appendBlade(std::vector<float>& mesh, const glm::vec3& base, float height, float width, float yaw, float lean, const glm::vec3& color) {
    const glm::vec3 side(std::cos(yaw), 0.0f, -std::sin(yaw));
    const glm::vec3 bend(std::sin(yaw) * lean, 0.0f, std::cos(yaw) * lean);
    auto spine = [&](float t) {
        return base + glm::vec3(0.0f, height * t, 0.0f) + bend * (height * t * t);
    };

    const glm::vec3 c0 = spine(0.0f);
    const glm::vec3 c1 = spine(0.5f);
    const glm::vec3 tip = spine(1.0f);
    const glm::vec3 baseColor = color * 0.55f;
    const glm::vec3 midColor = color;
    const glm::vec3 tipColor = glm::min(color * 1.15f, glm::vec3(1.0f));

    const glm::vec3 l0 = c0 - side * (width * 0.5f);
    const glm::vec3 r0 = c0 + side * (width * 0.5f);
    const glm::vec3 l1 = c1 - side * (width * 0.25f);
    const glm::vec3 r1 = c1 + side * (width * 0.25f);

    auto push = [&](const glm::vec3& p, const glm::vec3& c) {
        mesh.insert(mesh.end(), {p.x, p.y, p.z, c.r, c.g, c.b});
    };
    push(l0, baseColor); push(r0, baseColor); push(r1, midColor);
    push(l0, baseColor); push(r1, midColor); push(l1, midColor);
    push(l1, midColor); push(r1, midColor); push(tip, tipColor);
}

void appendGrassBlades(std::vector<float>& mesh, std::mt19937& engine) {
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);
    for (int i = 0; i < clumpCount; ++i) {
        const glm::vec3 clump(
            (2.0f * unit(engine) - 1.0f) * fieldHalfWidth,
            grassSurfaceY,
            (2.0f * unit(engine) - 1.0f) * fieldHalfDepth);
        if (insidePlayground(clump.x, clump.z)) {
            continue;
        }
        const float clumpYaw = randomBetween(engine, 0.0f, glm::two_pi<float>());
        const float clumpLean = randomBetween(engine, 0.05f, 0.3f);
        const float clumpShade = randomBetween(engine, 0.8f, 1.1f);
        const int blades = 4 + static_cast<int>(unit(engine) * 7.0f);
        for (int b = 0; b < blades; ++b) {
            const float angle = randomBetween(engine, 0.0f, glm::two_pi<float>());
            const float radius = clumpRadius * std::sqrt(unit(engine));
            const glm::vec3 base(clump.x + radius * std::cos(angle), grassSurfaceY, clump.z + radius * std::sin(angle));
            if (insidePlayground(base.x, base.z)) {
                continue;
            }
            const float height = randomBetween(engine, 0.15f, 0.42f);
            const float yaw = clumpYaw + randomBetween(engine, -0.5f, 0.5f);
            const float lean = clumpLean + randomBetween(engine, -0.05f, 0.1f);
            const float shade = clumpShade * randomBetween(engine, 0.9f, 1.1f);
            appendBlade(mesh, base, height, bladeWidth, yaw, lean, grassColor * shade);
        }
    }
}

void appendCornField(std::vector<float>& mesh, std::mt19937& engine) {
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);
    for (float z = -fieldHalfDepth + cornSpacing * 0.5f; z < fieldHalfDepth; z += cornSpacing) {
        for (float x = -fieldHalfWidth + cornSpacing * 0.5f; x < fieldHalfWidth; x += cornSpacing) {
            const float px = x + (unit(engine) - 0.5f) * cornSpacing * 0.4f;
            const float pz = z + (unit(engine) - 0.5f) * cornSpacing * 0.4f;
            if (insidePlayground(px, pz)) {
                continue;
            }
            const float height = 0.6f + 0.3f * unit(engine);
            const float shade = 0.85f + 0.3f * unit(engine);
            appendBox(mesh, glm::vec3(px, grassSurfaceY + height * 0.5f, pz), glm::vec3(cornStalkWidth, height, cornStalkWidth), cornStalkColor * shade);
            appendBox(mesh, glm::vec3(px, grassSurfaceY + height * 0.6f, pz), glm::vec3(cornLeafLength, 0.03f, cornStalkWidth * 1.5f), cornLeafColor * shade);
            appendBox(mesh, glm::vec3(px, grassSurfaceY + height + 0.05f, pz), glm::vec3(cornStalkWidth * 1.5f, 0.1f, cornStalkWidth * 1.5f), cornTasselColor * shade);
        }
    }
}

}

Ground::Ground() : VAO(0), VBO(0), sharedVertexCount(0), grassVertexCount(0), cornVertexCount(0) {
    std::vector<float> mesh;
    for (const auto& slab : slabs) {
        appendBox(mesh, slab.center, slab.size, slab.color);
    }
    appendBox(mesh, glm::vec3(0.0f, grassSurfaceY - 0.1f, 0.0f), glm::vec3(grassBaseSize, 0.2f, grassBaseSize), grassColor);
    sharedVertexCount = static_cast<int>(mesh.size() / 6);

    std::mt19937 engine(std::random_device{}());
    appendGrassBlades(mesh, engine);
    grassVertexCount = static_cast<int>(mesh.size() / 6) - sharedVertexCount;

    appendCornField(mesh, engine);
    cornVertexCount = static_cast<int>(mesh.size() / 6) - sharedVertexCount - grassVertexCount;

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

Ground::~Ground() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Ground::render(const Shader& shader, bool corn) const {
    shader.setMat4("uModel", glm::mat4(1.0f));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, sharedVertexCount);
    if (corn) {
        glDrawArrays(GL_TRIANGLES, sharedVertexCount + grassVertexCount, cornVertexCount);
    } else {
        glDrawArrays(GL_TRIANGLES, sharedVertexCount, grassVertexCount);
    }
    glBindVertexArray(0);
}

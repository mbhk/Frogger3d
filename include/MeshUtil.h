#ifndef MESH_UTIL_H
#define MESH_UTIL_H

#include <glm/glm.hpp>
#include <vector>

// Appends a colored box (36 vertices, position + color) to out
inline void appendBox(std::vector<float>& out, const glm::vec3& center, const glm::vec3& size, const glm::vec3& color) {
    const glm::vec3 h = size * 0.5f;
    const glm::vec3 corners[8] = {
        center + glm::vec3(-h.x, -h.y, -h.z),
        center + glm::vec3( h.x, -h.y, -h.z),
        center + glm::vec3( h.x,  h.y, -h.z),
        center + glm::vec3(-h.x,  h.y, -h.z),
        center + glm::vec3(-h.x, -h.y,  h.z),
        center + glm::vec3( h.x, -h.y,  h.z),
        center + glm::vec3( h.x,  h.y,  h.z),
        center + glm::vec3(-h.x,  h.y,  h.z),
    };
    const int triangles[12][3] = {
        {4, 5, 6}, {4, 6, 7},  // front
        {1, 0, 3}, {1, 3, 2},  // back
        {5, 1, 2}, {5, 2, 6},  // right
        {0, 4, 7}, {0, 7, 3},  // left
        {7, 6, 2}, {7, 2, 3},  // top
        {0, 1, 5}, {0, 5, 4},  // bottom
    };
    for (const auto& triangle : triangles) {
        for (int index : triangle) {
            const glm::vec3& p = corners[index];
            out.insert(out.end(), {p.x, p.y, p.z, color.r, color.g, color.b});
        }
    }
}

#endif // MESH_UTIL_H

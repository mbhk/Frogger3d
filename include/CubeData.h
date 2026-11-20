#ifndef CUBE_DATA_H
#define CUBE_DATA_H

// Cube vertices (position + color)
inline constexpr float cubeVertices[] = {
    // Front face (red)
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,

    // Back face (blue)
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

    // Top face (green)
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

    // Bottom face (yellow)
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,

    // Left face (magenta)
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,

    // Right face (cyan)
     0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f
};

// Cube indices for drawing
inline constexpr unsigned int cubeIndices[] = {
    0,  1,  2,  2,  3,  0,  // Front face
    4,  5,  6,  6,  7,  4,  // Back face
    8,  9, 10, 10, 11,  8,  // Top face
    12, 13, 14, 14, 15, 12, // Bottom face
    16, 17, 18, 18, 19, 16, // Left face
    20, 21, 22, 22, 23, 20  // Right face
};

#endif // CUBE_DATA_H

#ifndef FROG_DATA_H
#define FROG_DATA_H

// Tetrahedron vertices (position + color), 4 faces, 3 vertices each
inline constexpr float frogVertices[] = {
    // Face A-B-C
     0.5f,  0.5f,  0.5f, 0.2f, 0.8f, 0.2f,
     0.5f, -0.5f, -0.5f, 0.2f, 0.8f, 0.2f,
    -0.5f,  0.5f, -0.5f, 0.2f, 0.8f, 0.2f,

    // Face A-B-D
     0.5f,  0.5f,  0.5f, 0.1f, 0.6f, 0.1f,
     0.5f, -0.5f, -0.5f, 0.1f, 0.6f, 0.1f,
    -0.5f, -0.5f,  0.5f, 0.1f, 0.6f, 0.1f,

    // Face A-C-D
     0.5f,  0.5f,  0.5f, 0.3f, 0.9f, 0.3f,
    -0.5f,  0.5f, -0.5f, 0.3f, 0.9f, 0.3f,
    -0.5f, -0.5f,  0.5f, 0.3f, 0.9f, 0.3f,

    // Face B-C-D
     0.5f, -0.5f, -0.5f, 0.15f, 0.7f, 0.2f,
    -0.5f,  0.5f, -0.5f, 0.15f, 0.7f, 0.2f,
    -0.5f, -0.5f,  0.5f, 0.15f, 0.7f, 0.2f,
};

#endif // FROG_DATA_H

#include "Renderer.h"
#include "Window.h"
#include "Shader.h"
#include <iostream>

static const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vColor;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    vColor = aColor;
}
)";

static const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
)";

Renderer::Renderer(SDL_Window* window) : window(window) {
    // Set up OpenGL context attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    // Create OpenGL context
    context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Get window dimensions
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    this->width = width;
    this->height = height;

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW could not initialize! Error: " << glewGetErrorString(err) << std::endl;
    }

    // Set viewport
    setViewport(width, height);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    setClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    shader = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);
}

Renderer::~Renderer() {
    // Release GL objects while the context is still alive
    shader.reset();
    if (context) {
        SDL_GL_DeleteContext(context);
    }
}

void Renderer::clear() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::swap() const {
    SDL_GL_SwapWindow(window);
}

void Renderer::setViewport(int width, int height) const {
    glViewport(0, 0, width, height);
}

void Renderer::setClearColor(float r, float g, float b, float a) const {
    glClearColor(r, g, b, a);
}

void Renderer::setCamera(const glm::mat4& view, const glm::mat4& projection) const {
    shader->use();
    shader->setMat4("uView", view);
    shader->setMat4("uProjection", projection);
}

const Shader& Renderer::getShader() const {
    return *shader;
}

#include "Renderer.h"
#include <iostream>

Renderer::Renderer(int width, int height) : width(width), height(height) {
    // Set up OpenGL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    // Create OpenGL context
    context = std::unique_ptr<SDL_GLContext, decltype(&SDL_GL_DeleteContext)>(
        SDL_GL_CreateContext(window.get()),
        &SDL_GL_DeleteContext
    );
    
    if (!context) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW could not initialize! Error: " << glewGetErrorString(err) << std::endl;
    }

    // Set viewport
    setViewport(width, height);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() = default;

void Renderer::clear() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::swap() const {
    SDL_GL_SwapWindow(window.get());
}

void Renderer::setViewport(int width, int height) const {
    glViewport(0, 0, width, height);
}

void Renderer::setClearColor(float r, float g, float b, float a) const {
    glClearColor(r, g, b, a);
}
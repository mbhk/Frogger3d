#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void clear() const;
    void swap() const;
    void setViewport(int width, int height) const;
    void setClearColor(float r, float g, float b, float a) const;

private:
    std::unique_ptr<SDL_GLContext, decltype(&SDL_GL_DeleteContext)> context;
    int width;
    int height;
};

#endif // RENDERER_H
#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>

class Window;
class Shader;

class Renderer {
public:
    Renderer(SDL_Window* window);
    ~Renderer();

    void clear() const;
    void swap() const;
    void setViewport(int width, int height) const;
    void setClearColor(float r, float g, float b, float a) const;
    void setCamera(const glm::mat4& view, const glm::mat4& projection) const;
    void beginShadowPass() const;
    void endShadowPass() const;
    const Shader& getShader() const;

private:
    SDL_Window* window;
    SDL_GLContext context;
    std::unique_ptr<Shader> shader;
    int width;
    int height;
};

#endif // RENDERER_H

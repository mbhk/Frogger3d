#include "Window.h"
#include <iostream>

Window::Window(int width, int height, const char* title) 
    : window(SDL_CreateWindow(title, 
                            SDL_WINDOWPOS_UNDEFINED, 
                            SDL_WINDOWPOS_UNDEFINED,
                            width, height,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
            &SDL_DestroyWindow) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
}

Window::~Window() = default;

SDL_Window* Window::get() const {
    return window.get();
}

int Window::getWidth() const {
    int width = 0;
    int height = 0;
    SDL_GetWindowSize(window.get(), &width, &height);
    return width;
}

int Window::getHeight() const {
    int width = 0;
    int height = 0;
    SDL_GetWindowSize(window.get(), &width, &height);
    return height;
}

void Window::toggleFullscreen() {
    const Uint32 flags = SDL_GetWindowFlags(window.get());
    const bool fullscreen = (flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) != 0;
    SDL_SetWindowFullscreen(window.get(), fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void Window::swapBuffers() {
    SDL_GL_SwapWindow(window.get());
}
#include "Window.h"
#include <iostream>

Window::Window(int width, int height, const char* title) 
    : window(SDL_CreateWindow(title, 
                            SDL_WINDOWPOS_UNDEFINED, 
                            SDL_WINDOWPOS_UNDEFINED,
                            width, height,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN),
            &SDL_DestroyWindow),
      width(width), 
      height(height) {
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
    return width;
}

int Window::getHeight() const {
    return height;
}

void Window::swapBuffers() {
    SDL_GL_SwapWindow(window.get());
}
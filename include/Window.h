#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <memory>

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    SDL_Window* get() const;
    int getWidth() const;
    int getHeight() const;
    void swapBuffers();

private:
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
    int width;
    int height;
};

#endif // WINDOW_H
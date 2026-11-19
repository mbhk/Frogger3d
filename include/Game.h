#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

class Window;
class Renderer;
class Player;
class Enemy;

class Game {
public:
    Game();
    ~Game();

    void run();
    void handleEvents();
    void update(float deltaTime);
    void render();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    bool isRunning;
    SDL_Event event;
};

#endif // GAME_H
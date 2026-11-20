#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

class Window;
class Renderer;
class TextRenderer;
class Player;
class Enemy;

class Game {
public:
    Game();
    ~Game();

    void run();
    void handleEvents();
    void handleKeyPress(SDL_Keycode key);
    void moveAcrossFields(int step);
    void moveInDepth(float dz);
    float fieldCenterX(int field) const;
    bool isPaused() const;
    void update(float deltaTime);
    void render();
    void renderHUD();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<TextRenderer> textRenderer;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    bool isRunning;
    bool paused;
    bool showHelp;
    int currentField;
    SDL_Event event;
};

#endif // GAME_H

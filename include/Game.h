#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <vector>

class Window;
class Renderer;
class TextRenderer;
class Ground;
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
    void moveRows(int step);
    float fieldCenterX(int field) const;
    void placeFrog();
    void resetFrog();
    void checkCollisions();
    void showStatus(const std::string& message, float duration);
    bool isPaused() const;
    void update(float deltaTime);
    void render();
    void renderHUD();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<TextRenderer> textRenderer;
    std::unique_ptr<Ground> ground;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    bool isRunning;
    bool paused;
    bool showHelp;
    int currentField;
    int currentRow;
    std::string statusMessage;
    float statusTimer;
    SDL_Event event;
};

#endif // GAME_H

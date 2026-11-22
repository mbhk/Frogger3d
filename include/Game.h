#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <memory>
#include <random>
#include <string>
#include <vector>

class Window;
class Renderer;
class TextRenderer;
class Ground;
class Lamp;
class Crossing;
class PedestrianLight;
class Player;
class Enemy;
class Pickup;

struct TrafficLane {
    int row;
    float direction;
    int speedStep;
    std::vector<std::unique_ptr<Enemy>> cars;
};

class Game {
public:
    Game();
    ~Game();

    void run();
    void handleEvents();
    void handleKeyPress(SDL_Keycode key);
    void restartGame();
    void spawnCar(TrafficLane& lane, float x);
    void applyCarSpeed();
    void speedUpRandomLane();
    void updateTraffic(float deltaTime);
    void updateHeart(float deltaTime);
    void spawnHeart();
    void collectHeart();
    void pressCrossingButton();
    void placeCrossing();
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
    std::vector<std::unique_ptr<Lamp>> lamps;
    std::unique_ptr<Crossing> crossing;
    std::unique_ptr<PedestrianLight> pedestrianLight;
    std::unique_ptr<Player> player;
    std::vector<TrafficLane> lanes;
    std::unique_ptr<Pickup> heart;
    bool isRunning;
    bool paused;
    bool showHelp;
    bool gameOver;
    bool nightTime;
    bool cornField;
    int currentField;
    int currentRow;
    int level;
    int lives;
    std::string statusMessage;
    float statusTimer;
    float respawnTimer;
    float heartTimer;
    int carLimit;
    std::mt19937 randomEngine;
    SDL_Event event;
};

#endif // GAME_H

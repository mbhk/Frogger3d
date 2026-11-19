#include "Game.h"
#include "Window.h"
#include "Renderer.h"
#include "Player.h"
#include "Enemy.h"
#include <iostream>
#include <memory>
#include <chrono>

Game::Game() : isRunning(false) {
    window = std::make_unique<Window>(800, 600, "3D Frogger");
    renderer = std::make_unique<Renderer>(800, 600);
    isRunning = true;
    
    // Create a simple player
    player = std::make_unique<Player>(0.0f, 0.0f, 0.0f);
    
    // Create some enemies
    enemies.push_back(std::make_unique<Enemy>(2.0f, 0.0f, 0.0f));
    enemies.push_back(std::make_unique<Enemy>(-2.0f, 0.0f, 0.0f));
}

Game::~Game() = default;

void Game::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (isRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        handleEvents();
        update(deltaTime);
        render();
    }
}

void Game::handleEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
    }
}

void Game::update(float deltaTime) {
    // Update player position
    if (player) {
        player->update(deltaTime);
    }
    
    // Update enemies
    for (auto& enemy : enemies) {
        if (enemy) {
            enemy->update(deltaTime);
        }
    }
}

void Game::render() {
    renderer->clear();
    
    // Render player
    if (player) {
        player->render();
    }
    
    // Render enemies
    for (auto& enemy : enemies) {
        if (enemy) {
            enemy->render();
        }
    }
    
    renderer->swap();
}
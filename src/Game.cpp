#include "Game.h"
#include "Window.h"
#include "Renderer.h"
#include "Player.h"
#include "Enemy.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <chrono>

Game::Game() : isRunning(false) {
    window = std::make_unique<Window>(800, 600, "3D Frogger");
    renderer = std::make_unique<Renderer>(window->get());
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

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 6.0f, 8.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float aspect = static_cast<float>(window->getWidth()) / static_cast<float>(window->getHeight());
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    renderer->setCamera(view, projection);
    
    // Render player
    if (player) {
        player->render(renderer->getShader());
    }
    
    // Render enemies
    for (auto& enemy : enemies) {
        if (enemy) {
            enemy->render(renderer->getShader());
        }
    }
    
    renderer->swap();
}
#include "Game.h"
#include "Window.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "Player.h"
#include "Enemy.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <chrono>

namespace {

const float boardLimit = 5.0f;
const int fieldCount = 15;
const int startField = fieldCount / 2;
const float cameraDistance = 10.0f;
const float cameraFovY = 45.0f;
const float hudScale = 2.0f;
const float hudPadding = 6.0f;
const float helpPadding = 16.0f;
const float lineSpacing = 10.0f;
const glm::vec4 textColor(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 accentColor(1.0f, 0.9f, 0.3f, 1.0f);
const glm::vec4 dimColor(0.7f, 0.7f, 0.7f, 1.0f);
const glm::vec4 panelColor(0.0f, 0.0f, 0.0f, 0.7f);

const std::vector<std::string> helpLines = {
    "W / UP ARROW      MOVE FORWARD",
    "S / DOWN ARROW    MOVE BACK",
    "A / LEFT ARROW    ONE FIELD LEFT",
    "D / RIGHT ARROW   ONE FIELD RIGHT",
    "P                 PAUSE / RESUME",
    "H / ?             TOGGLE HELP",
    "Q                 QUIT",
};

}

Game::Game() : isRunning(false), paused(false), showHelp(false), currentField(startField) {
    window = std::make_unique<Window>(800, 600, "3D Frogger");
    renderer = std::make_unique<Renderer>(window->get());
    textRenderer = std::make_unique<TextRenderer>();
    isRunning = true;
    
    // Create a simple player in the middle field
    player = std::make_unique<Player>(fieldCenterX(currentField), 0.0f, 0.0f);
    
    // Create some enemies
    enemies.push_back(std::make_unique<Enemy>(2.0f, 0.0f, 0.0f, glm::vec3(0.9f, 0.2f, 0.2f)));
    enemies.push_back(std::make_unique<Enemy>(-2.0f, 0.0f, 0.0f, glm::vec3(0.2f, 0.5f, 0.9f)));
}

Game::~Game() = default;

void Game::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (isRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        handleEvents();
        if (!isPaused()) {
            update(deltaTime);
        }
        render();
    }
}

void Game::handleEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        } else if (event.type == SDL_KEYDOWN && !event.key.repeat) {
            handleKeyPress(event.key.keysym.sym);
        }
    }
}

void Game::handleKeyPress(SDL_Keycode key) {
    switch (key) {
        case SDLK_q:
            isRunning = false;
            break;
        case SDLK_p:
            paused = !paused;
            break;
        case SDLK_h:
        case SDLK_QUESTION:
            showHelp = !showHelp;
            break;
        case SDLK_w:
        case SDLK_UP:
            moveInDepth(-1.0f);
            break;
        case SDLK_s:
        case SDLK_DOWN:
            moveInDepth(1.0f);
            break;
        case SDLK_a:
        case SDLK_LEFT:
            moveAcrossFields(-1);
            break;
        case SDLK_d:
        case SDLK_RIGHT:
            moveAcrossFields(1);
            break;
        default:
            break;
    }
}

void Game::moveAcrossFields(int step) {
    if (!player || isPaused()) {
        return;
    }
    currentField = std::clamp(currentField + step, 0, fieldCount - 1);
    glm::vec3 position = player->getPosition();
    player->setPosition(fieldCenterX(currentField), position.y, position.z);
}

void Game::moveInDepth(float dz) {
    if (!player || isPaused()) {
        return;
    }
    glm::vec3 position = player->getPosition();
    player->setPosition(position.x, position.y, std::clamp(position.z + dz, -boardLimit, boardLimit));
}

float Game::fieldCenterX(int field) const {
    const float aspect = static_cast<float>(window->getWidth()) / static_cast<float>(window->getHeight());
    const float halfWidth = std::tan(glm::radians(cameraFovY) * 0.5f) * aspect * cameraDistance;
    const float fieldWidth = 2.0f * halfWidth / fieldCount;
    return -halfWidth + (field + 0.5f) * fieldWidth;
}

bool Game::isPaused() const {
    return paused || showHelp;
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

    renderHUD();
    
    renderer->swap();
}

void Game::renderHUD() {
    const float width = static_cast<float>(window->getWidth());
    const float height = static_cast<float>(window->getHeight());
    const float charHeight = 7.0f * hudScale;
    const float lineHeight = charHeight + lineSpacing;

    textRenderer->begin(window->getWidth(), window->getHeight());

    const std::string hint = "H FOR HELP";
    const float hintWidth = textRenderer->textWidth(hint, hudScale);
    const float hintX = width - hintWidth - hudPadding - 10.0f;
    const float hintY = 10.0f;
    textRenderer->drawRect(hintX - hudPadding, hintY - hudPadding, hintWidth + 2 * hudPadding, charHeight + 2 * hudPadding, panelColor);
    textRenderer->drawText(hint, hintX, hintY, hudScale, accentColor);

    const std::string placement = "FIELD " + std::to_string(currentField + 1) + " / " + std::to_string(fieldCount);
    const float placementX = hudPadding + 10.0f;
    const float placementY = height - charHeight - hudPadding - 10.0f;
    textRenderer->drawRect(placementX - hudPadding, placementY - hudPadding, textRenderer->textWidth(placement, hudScale) + 2 * hudPadding, charHeight + 2 * hudPadding, panelColor);
    textRenderer->drawText(placement, placementX, placementY, hudScale, accentColor);

    if (showHelp) {
        const std::string title = "CONTROLS";
        const std::string footer = "PRESS H OR ? TO CLOSE";

        float contentWidth = std::max(textRenderer->textWidth(title, hudScale), textRenderer->textWidth(footer, hudScale));
        for (const auto& line : helpLines) {
            contentWidth = std::max(contentWidth, textRenderer->textWidth(line, hudScale));
        }

        const float panelWidth = contentWidth + 2 * helpPadding;
        const float panelHeight = (helpLines.size() + 4) * lineHeight + 2 * helpPadding;
        const float panelX = (width - panelWidth) / 2.0f;
        const float panelY = (height - panelHeight) / 2.0f;

        textRenderer->drawRect(panelX, panelY, panelWidth, panelHeight, panelColor);

        float y = panelY + helpPadding;
        textRenderer->drawText(title, panelX + helpPadding, y, hudScale, accentColor);
        y += 2 * lineHeight;
        for (const auto& line : helpLines) {
            textRenderer->drawText(line, panelX + helpPadding, y, hudScale, textColor);
            y += lineHeight;
        }
        y += lineHeight;
        textRenderer->drawText(footer, panelX + helpPadding, y, hudScale, dimColor);
    } else if (paused) {
        const std::string message = "PAUSED";
        const float messageScale = 4.0f;
        const float messageWidth = textRenderer->textWidth(message, messageScale);
        const float messageHeight = 7.0f * messageScale;
        const float messageX = (width - messageWidth) / 2.0f;
        const float messageY = (height - messageHeight) / 2.0f;

        textRenderer->drawRect(messageX - helpPadding, messageY - helpPadding, messageWidth + 2 * helpPadding, messageHeight + 2 * helpPadding, panelColor);
        textRenderer->drawText(message, messageX, messageY, messageScale, accentColor);
    }

    textRenderer->end();
}

#include "Game.h"
#include "Window.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "Ground.h"
#include "Player.h"
#include "Enemy.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <chrono>

namespace {

const int fieldCount = 15;
const int startField = fieldCount / 2;
const int rowCount = 6;
const int startRow = 0;
const int goalRow = rowCount - 1;
const float rowZeroZ = 2.5f;
const float frogHalfSize = 0.5f;
const float cameraDistance = 10.0f;
const float cameraFovY = 45.0f;
const float hudScale = 2.0f;
const float hudPadding = 6.0f;
const float helpPadding = 16.0f;
const float lineSpacing = 10.0f;
const float statusScale = 3.0f;
const float winMessageTime = 2.0f;
const float hitMessageTime = 1.5f;
const int startLevel = 1;
const int startLives = 3;
const glm::vec4 textColor(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 accentColor(1.0f, 0.9f, 0.3f, 1.0f);
const glm::vec4 dimColor(0.7f, 0.7f, 0.7f, 1.0f);
const glm::vec4 panelColor(0.0f, 0.0f, 0.0f, 0.7f);

const std::vector<std::string> helpLines = {
    "W / UP ARROW      ONE ROW FORWARD",
    "S / DOWN ARROW    ONE ROW BACK",
    "A / LEFT ARROW    ONE FIELD LEFT",
    "D / RIGHT ARROW   ONE FIELD RIGHT",
    "P                 PAUSE / RESUME",
    "H / ?             TOGGLE HELP",
    "SPACE             START OVER (GAME OVER)",
    "Q                 QUIT",
};

float rowCenterZ(int row) {
    return rowZeroZ - static_cast<float>(row);
}

}

Game::Game() : isRunning(false), paused(false), showHelp(false), gameOver(false), currentField(startField), currentRow(startRow), level(startLevel), lives(startLives), statusTimer(0.0f) {
    window = std::make_unique<Window>(800, 600, "3D Frogger");
    renderer = std::make_unique<Renderer>(window->get());
    textRenderer = std::make_unique<TextRenderer>();
    ground = std::make_unique<Ground>();
    isRunning = true;
    
    // Create the frog on the lower pavement
    player = std::make_unique<Player>(fieldCenterX(currentField), 0.0f, rowCenterZ(currentRow));
    
    // Create cars in two road lanes
    enemies.push_back(std::make_unique<Enemy>(2.0f, 0.0f, rowCenterZ(2), glm::vec3(0.9f, 0.2f, 0.2f)));
    enemies.push_back(std::make_unique<Enemy>(-2.0f, 0.0f, rowCenterZ(4), glm::vec3(0.2f, 0.5f, 0.9f)));
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
        case SDLK_SPACE:
            if (gameOver) {
                restartGame();
            }
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
            moveRows(1);
            break;
        case SDLK_s:
        case SDLK_DOWN:
            moveRows(-1);
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

void Game::restartGame() {
    gameOver = false;
    level = startLevel;
    lives = startLives;
    statusTimer = 0.0f;
    resetFrog();
}

void Game::moveAcrossFields(int step) {
    if (!player || isPaused()) {
        return;
    }
    currentField = std::clamp(currentField + step, 0, fieldCount - 1);
    placeFrog();
}

void Game::moveRows(int step) {
    if (!player || isPaused()) {
        return;
    }
    currentRow = std::clamp(currentRow + step, startRow, goalRow);
    if (currentRow == goalRow) {
        level++;
        showStatus("YOU MADE IT!", winMessageTime);
        resetFrog();
        return;
    }
    placeFrog();
}

float Game::fieldCenterX(int field) const {
    const float aspect = static_cast<float>(window->getWidth()) / static_cast<float>(window->getHeight());
    const float halfWidth = std::tan(glm::radians(cameraFovY) * 0.5f) * aspect * cameraDistance;
    const float fieldWidth = 2.0f * halfWidth / fieldCount;
    return -halfWidth + (field + 0.5f) * fieldWidth;
}

void Game::placeFrog() {
    player->setPosition(fieldCenterX(currentField), 0.0f, rowCenterZ(currentRow));
}

void Game::resetFrog() {
    currentField = startField;
    currentRow = startRow;
    placeFrog();
}

void Game::checkCollisions() {
    if (!player) {
        return;
    }
    const glm::vec3 frog = player->getPosition();
    for (const auto& enemy : enemies) {
        if (!enemy) {
            continue;
        }
        const glm::vec3 car = enemy->getPosition();
        const glm::vec3 carHalf = enemy->getHalfExtents();
        if (std::abs(frog.x - car.x) < carHalf.x + frogHalfSize &&
            std::abs(frog.z - car.z) < carHalf.z + frogHalfSize) {
            lives--;
            if (lives <= 0) {
                lives = 0;
                gameOver = true;
            } else {
                showStatus("SPLAT! TRY AGAIN", hitMessageTime);
                resetFrog();
            }
            return;
        }
    }
}

void Game::showStatus(const std::string& message, float duration) {
    statusMessage = message;
    statusTimer = duration;
}

bool Game::isPaused() const {
    return paused || showHelp || gameOver;
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

    if (statusTimer > 0.0f) {
        statusTimer -= deltaTime;
    }

    checkCollisions();
}

void Game::render() {
    renderer->clear();

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 6.0f, 8.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float aspect = static_cast<float>(window->getWidth()) / static_cast<float>(window->getHeight());
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    renderer->setCamera(view, projection);

    // Render pavements and road
    ground->render(renderer->getShader());
    
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

    const std::string levelText = "LEVEL " + std::to_string(level);
    const std::string livesText = "LIVES " + std::to_string(lives);
    const float infoX = hudPadding + 10.0f;
    const float infoY = 10.0f;
    const float infoWidth = std::max(textRenderer->textWidth(levelText, hudScale), textRenderer->textWidth(livesText, hudScale));
    const float infoHeight = 2 * charHeight + lineSpacing;
    textRenderer->drawRect(infoX - hudPadding, infoY - hudPadding, infoWidth + 2 * hudPadding, infoHeight + 2 * hudPadding, panelColor);
    textRenderer->drawText(levelText, infoX, infoY, hudScale, accentColor);
    textRenderer->drawText(livesText, infoX, infoY + charHeight + lineSpacing, hudScale, accentColor);

    if (statusTimer > 0.0f && !showHelp && !gameOver) {
        const float statusHeight = 7.0f * statusScale;
        const float statusWidth = textRenderer->textWidth(statusMessage, statusScale);
        const float statusX = (width - statusWidth) / 2.0f;
        const float statusY = 60.0f;
        textRenderer->drawRect(statusX - helpPadding, statusY - helpPadding, statusWidth + 2 * helpPadding, statusHeight + 2 * helpPadding, panelColor);
        textRenderer->drawText(statusMessage, statusX, statusY, statusScale, accentColor);
    }

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
    } else if (gameOver) {
        const std::string message = "GAME OVER";
        const std::string hint = "SPACE START OVER OR Q FOR QUIT";
        const float messageScale = 4.0f;
        const float messageWidth = textRenderer->textWidth(message, messageScale);
        const float messageHeight = 7.0f * messageScale;
        const float hintWidth = textRenderer->textWidth(hint, hudScale);
        const float boxWidth = std::max(messageWidth, hintWidth) + 2 * helpPadding;
        const float boxHeight = messageHeight + lineHeight + 2 * helpPadding;
        const float boxX = (width - boxWidth) / 2.0f;
        const float boxY = (height - boxHeight) / 2.0f;

        textRenderer->drawRect(boxX, boxY, boxWidth, boxHeight, panelColor);
        textRenderer->drawText(message, (width - messageWidth) / 2.0f, boxY + helpPadding, messageScale, accentColor);
        textRenderer->drawText(hint, (width - hintWidth) / 2.0f, boxY + helpPadding + messageHeight + lineSpacing, hudScale, textColor);
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

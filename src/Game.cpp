#include "Game.h"
#include "Window.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "Ground.h"
#include "Lamp.h"
#include "Light.h"
#include "Player.h"
#include "Enemy.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>
#include <chrono>

namespace {

const int fieldCount = 15;
const int startField = fieldCount / 2;
const int rowCount = 6;
const int startRow = 0;
const int goalRow = rowCount - 1;
const float rowZeroZ = 2.5f;
const float frogHalfSize = 0.3f;
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
const float carSpawnX = 8.5f;
const float minSpawnGap = 2.5f;
const float respawnMinTime = 0.3f;
const float respawnMaxTime = 1.0f;
const float baseCarSpeed = 3.0f;
const float carSpeedPerStep = 0.6f;
const int maxSpeedStepDifference = 3;
const int levelsPerExtraCar = 4;
const int startCarLimit = 1;
const glm::vec4 textColor(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 accentColor(1.0f, 0.9f, 0.3f, 1.0f);
const glm::vec4 dimColor(0.7f, 0.7f, 0.7f, 1.0f);
const glm::vec4 panelColor(0.0f, 0.0f, 0.0f, 0.7f);
const glm::vec3 cameraPosition(0.0f, 6.0f, 8.0f);
const float lampOffsetsX[] = {-4.0f, 4.0f};
const glm::vec3 dayAmbientColor(0.35f, 0.35f, 0.35f);
const glm::vec3 daySunColor(0.7f, 0.68f, 0.6f);
const glm::vec3 nightAmbientColor(0.08f, 0.08f, 0.14f);
const glm::vec3 nightSunColor(0.0f, 0.0f, 0.0f);
const glm::vec3 sunDirection = glm::normalize(glm::vec3(0.3f, 1.0f, 0.5f));
const std::size_t maxLights = 32;

const std::vector<std::string> helpLines = {
    "W / UP ARROW      ONE ROW FORWARD",
    "S / DOWN ARROW    ONE ROW BACK",
    "A / LEFT ARROW    ONE FIELD LEFT",
    "D / RIGHT ARROW   ONE FIELD RIGHT",
    "P                 PAUSE / RESUME",
    "N                 TOGGLE DAY / NIGHT",
    "C                 TOGGLE GRASS / CORN",
    "H / ?             TOGGLE HELP",
    "SPACE             START OVER (GAME OVER)",
    "Q                 QUIT",
};

float rowCenterZ(int row) {
    return rowZeroZ - static_cast<float>(row);
}

// Even road lanes drive right to left, odd road lanes drive left to right
float laneDirection(int row) {
    return row % 2 == 0 ? -1.0f : 1.0f;
}

float carSpeed(const TrafficLane& lane) {
    return baseCarSpeed + carSpeedPerStep * static_cast<float>(lane.speedStep);
}

// True if no car in the lane is close enough to the entry point to collide with a newly spawned car
bool isEntryClear(const TrafficLane& lane, float entryX) {
    for (const auto& car : lane.cars) {
        if (std::abs(car->getPosition().x - entryX) < minSpawnGap) {
            return false;
        }
    }
    return true;
}

// True if raising the lane at index by one step keeps all lanes within maxSpeedStepDifference
bool canIncreaseStep(const std::vector<TrafficLane>& lanes, std::size_t index) {
    int lowest = lanes[index].speedStep + 1;
    int highest = lowest;
    for (std::size_t i = 0; i < lanes.size(); ++i) {
        if (i != index) {
            lowest = std::min(lowest, lanes[i].speedStep);
            highest = std::max(highest, lanes[i].speedStep);
        }
    }
    return highest - lowest <= maxSpeedStepDifference;
}

float randomBetween(std::mt19937& engine, float low, float high) {
    std::uniform_real_distribution<float> distribution(low, high);
    return distribution(engine);
}

std::size_t randomIndex(std::mt19937& engine, std::size_t count) {
    std::uniform_int_distribution<std::size_t> distribution(0, count - 1);
    return distribution(engine);
}

}

Game::Game() : isRunning(false), paused(false), showHelp(false), gameOver(false), currentField(startField), currentRow(startRow), level(startLevel), lives(startLives), statusTimer(0.0f), respawnTimer(0.0f), carLimit(startCarLimit), nightTime(false), cornField(false), randomEngine(std::random_device{}()) {
    window = std::make_unique<Window>(800, 600, "3D Frogger");
    renderer = std::make_unique<Renderer>(window->get());
    textRenderer = std::make_unique<TextRenderer>();
    ground = std::make_unique<Ground>();

    // Two lamps on each pavement, slightly shifted from each other
    for (int row : {startRow, goalRow}) {
        for (float offsetX : lampOffsetsX) {
            lamps.push_back(std::make_unique<Lamp>(glm::vec3(offsetX, 0.0f, rowCenterZ(row))));
        }
    }
    isRunning = true;
    
    // Create the frog on the lower pavement
    player = std::make_unique<Player>(fieldCenterX(currentField), 0.0f, rowCenterZ(currentRow));
    
    // One traffic lane per road row; the first car starts after the first respawn delay
    for (int row = 1; row < goalRow; ++row) {
        lanes.push_back({row, laneDirection(row), 0, {}});
    }
}

void Game::spawnCar(TrafficLane& lane, float x) {
    const glm::vec3 color(
        randomBetween(randomEngine, 0.2f, 1.0f),
        randomBetween(randomEngine, 0.2f, 1.0f),
        randomBetween(randomEngine, 0.2f, 1.0f));
    lane.cars.push_back(std::make_unique<Enemy>(x, 0.0f, rowCenterZ(lane.row), color, lane.direction));
    lane.cars.back()->setSpeed(carSpeed(lane));
}

void Game::applyCarSpeed() {
    for (auto& lane : lanes) {
        for (auto& car : lane.cars) {
            car->setSpeed(carSpeed(lane));
        }
    }
}

void Game::speedUpRandomLane() {
    std::size_t chosen = randomIndex(randomEngine, lanes.size());
    if (!canIncreaseStep(lanes, chosen)) {
        std::vector<std::size_t> candidates;
        for (std::size_t i = 0; i < lanes.size(); ++i) {
            if (canIncreaseStep(lanes, i)) {
                candidates.push_back(i);
            }
        }
        chosen = candidates[randomIndex(randomEngine, candidates.size())];
    }
    lanes[chosen].speedStep++;
}

void Game::updateTraffic(float deltaTime) {
    for (auto& lane : lanes) {
        for (auto& car : lane.cars) {
            car->update(deltaTime);
        }

        const std::size_t carsBefore = lane.cars.size();
        lane.cars.erase(std::remove_if(lane.cars.begin(), lane.cars.end(), [](const std::unique_ptr<Enemy>& car) {
            return car->isPastExit();
        }), lane.cars.end());
        if (lane.cars.size() != carsBefore) {
            respawnTimer = randomBetween(randomEngine, respawnMinTime, respawnMaxTime);
        }
    }

    int carsOnRoad = 0;
    for (const auto& lane : lanes) {
        carsOnRoad += static_cast<int>(lane.cars.size());
    }
    if (carsOnRoad >= carLimit) {
        return;
    }

    respawnTimer -= deltaTime;
    if (respawnTimer > 0.0f) {
        return;
    }

    std::vector<std::size_t> freeLanes;
    for (std::size_t i = 0; i < lanes.size(); ++i) {
        if (isEntryClear(lanes[i], -lanes[i].direction * carSpawnX)) {
            freeLanes.push_back(i);
        }
    }
    if (freeLanes.empty()) {
        return;
    }

    TrafficLane& lane = lanes[freeLanes[randomIndex(randomEngine, freeLanes.size())]];
    spawnCar(lane, -lane.direction * carSpawnX);
    respawnTimer = randomBetween(randomEngine, respawnMinTime, respawnMaxTime);
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
        case SDLK_n:
            nightTime = !nightTime;
            break;
        case SDLK_c:
            cornField = !cornField;
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
    for (auto& lane : lanes) {
        lane.speedStep = 0;
        lane.cars.clear();
    }
    carLimit = startCarLimit;
    respawnTimer = 0.0f;
    applyCarSpeed();
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
        if ((level - startLevel) % levelsPerExtraCar == 0) {
            carLimit++;
        } else {
            speedUpRandomLane();
        }
        applyCarSpeed();
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
    player->hopTo(fieldCenterX(currentField), 0.0f, rowCenterZ(currentRow));
}

void Game::resetFrog() {
    currentField = startField;
    currentRow = startRow;
    player->setPosition(fieldCenterX(currentField), 0.0f, rowCenterZ(currentRow));
}

void Game::checkCollisions() {
    if (!player) {
        return;
    }
    const glm::vec3 frog = player->getPosition();
    for (const auto& lane : lanes) {
        for (const auto& enemy : lane.cars) {
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
    
    updateTraffic(deltaTime);

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

    std::vector<PointLight> lights;
    if (nightTime) {
        for (const auto& lamp : lamps) {
            lights.push_back(lamp->getLight());
        }
        for (const auto& lane : lanes) {
            for (const auto& car : lane.cars) {
                const std::vector<PointLight> carLights = car->getLights();
                lights.insert(lights.end(), carLights.begin(), carLights.end());
            }
        }
    }

    const Shader& shader = renderer->getShader();
    shader.setVec3("uViewPos", cameraPosition);
    shader.setVec3("uAmbientColor", nightTime ? nightAmbientColor : dayAmbientColor);
    shader.setVec3("uSunDirection", sunDirection);
    shader.setVec3("uSunColor", nightTime ? nightSunColor : daySunColor);
    const std::size_t lightCount = std::min(lights.size(), maxLights);
    shader.setInt("uLightCount", static_cast<int>(lightCount));
    for (std::size_t i = 0; i < lightCount; ++i) {
        shader.setVec3("uLightPos[" + std::to_string(i) + "]", lights[i].position);
        shader.setVec3("uLightColor[" + std::to_string(i) + "]", lights[i].color);
    }

    // Render pavements and road
    ground->render(shader, cornField);

    for (auto& lamp : lamps) {
        lamp->render(shader);
    }
    
    // Render player
    if (player) {
        player->render(renderer->getShader());
    }
    
    // Render cars
    for (auto& lane : lanes) {
        for (auto& car : lane.cars) {
            car->render(renderer->getShader());
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

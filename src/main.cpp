#include "Game.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Starting 3D Frogger game..." << std::endl;
    
    // Initialize SDL first
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    try {
        Game game;
        std::cout << "Game initialized successfully, starting main loop..." << std::endl;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
        return 1;
    }
    
    std::cout << "Game ended normally." << std::endl;
    SDL_Quit();
    return 0;
}